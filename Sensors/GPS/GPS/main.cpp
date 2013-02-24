#define _WIN32_WINNT 0x0500    //to use GetConsoleWindow() function

#include "Common.h"
#include "GPSTransforms.h"
#include "qextserialport.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <ctime>
#include <windows.h>


using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Conversions;
using namespace std;

// serial port constants
const int BUFFER_LENGTH = 10000;    // The size of the receive buffer in bytes
const int READ_INTERVAL = 10;         // How frequently to read from the gps in ms
const int RECONNECT_INTERVAL = 100;   // How frequently to attempt to connect to the gps in ms. Currently not implemented
const int TIMEOUT_INTERVAL = 1000;     // Do a reset if no response for 500ms
int tick_interval = READ_INTERVAL;
CPerformanceTimer *intervalTimer;
long timeout = 0;

// message info
//const int MESSAGE_SIZE = 18;  //18 bytes per message
const int MAX_MESSAGE_SIZE = 120; //gps sends variable length strings
const char MESSAGE_START_BYTE = 0x24;

// ipc message 
GPSUpdateType update = {0, 0, 0, 0, 0};
RawGPSUpdateType rawUpdate = {0, 0, 0, 0, 0};

// serial port reading
QextSerialPort *serialPort;
char buffer[BUFFER_LENGTH + 1];
vector<char> megaBuffer;
vector<string> lines;    
long readLength;

// logging
bool logging = false;
ofstream logfile;
const char *LOGGING_ROOT_GPS = "C:\\IGVC\\Logs\\";

long count = 0;
double degrees, minutes;


BOOL CtrlHandler( DWORD fdwCtrlType ) 
{
	if(serialPort->isOpen())
		serialPort->close();
    logfile.close();
    return FALSE;
}


void resetPort()
{
	cout << "Resetting port\n";
	if(serialPort == NULL)
		return;
	serialPort->close();
	serialPort->open(QIODevice::ReadOnly);
}


template <class T>
bool from_string(T& t, 
                 const std::string& s, 
                 std::ios_base& (*f)(std::ios_base&))
{
  std::istringstream iss(s);
  return !(iss >> f >> t).fail();
}


// Extracts the next double in the string s, stores it in value
// updates the iterator it to the new position, and returns 0 on failure
int getNextDoubleInString(string &s, double &value, string::iterator &it)
{
	while (!((*it >= 48 && *it <= 57) || *it == 45 || *it == 46)) {    //increment it until we hit a number or - or .
		++it;
		if (it == s.end())
			return 0;
	}

	string strBuffer;
	while ((*it >= 48 && *it <= 57) || *it == 45 || *it == 46) {
		strBuffer.push_back(*it);
		++it;
		if (it == s.end())
			break;
	}

	if (from_string<double>(value, strBuffer, std::dec))
		return 1;
	else
		return 0;
}



void timerHandler()
{
    intervalTimer->Stop();
    double dt = intervalTimer->Interval_mS();
    timeout += dt;
    if (timeout > TIMEOUT_INTERVAL) {
        cout << "Serial port timed-out. Resetting..." << endl;
        resetPort();
        timeout = 0;
    }
    intervalTimer->Start();
    

	if(!serialPort->isOpen())
	{
		cout << "Serial Port Not Open" << endl;
		return;
	}

    // read and append to megaBuffer
    bool readSomething = false;
    while((readLength = serialPort->readLine(buffer, BUFFER_LENGTH)) > 0)
    {
        readSomething = true;
        for (int i = 0; i < readLength; ++i)
            megaBuffer.push_back(buffer[i]);
    }
    if (readSomething) {
        //reset the timeout timer
        timeout = 0;
        intervalTimer->Stop();
        intervalTimer->Start();
    }

    // extract lines from megaBuffer
	// this only reads GPRMC messages, so I won't bother reading the tags
    vector<char>::iterator it;
    for (it = megaBuffer.begin(); megaBuffer.end() - it > MAX_MESSAGE_SIZE ; ++it)
    {
        if (*it == MESSAGE_START_BYTE) {
            char checksum = 0;
			char a;
			int i;
            for (i = 1; (a = *(it+i)) != '*' && i < MAX_MESSAGE_SIZE; ++i) {
                //The checksum is the hex xor sum of the characters between the
				//$ and the *. This is the NMEA standard format.
				checksum ^= a;
			}
			
			if (i == MAX_MESSAGE_SIZE) continue;   //no end of message found
           
			string checksumStr;
			int checksumChar;
			checksumStr.push_back('0');
			checksumStr.push_back('x');
			checksumStr.push_back(*(it+i+1));
			checksumStr.push_back(*(it+i+2));
			stringstream convert(checksumStr);
			convert >> std::hex >> checksumChar;

            if  (checksum != checksumChar)     //checksum not correct
                continue;

            lines.push_back(string(it, it + i - 1));
            it = it + i + 2;    //the ++i in the for loop increments one more
        }
    }
    if (it != megaBuffer.end()) {
        vector<char> temp(it, megaBuffer.end());
        megaBuffer = temp;
    } else {
        megaBuffer.clear();
    }

	if (lines.size() == 0)
		return;

	//only parse the last line
	string tempStr = lines[lines.size()-1]; 
	vector<string> fields;
	Data::stringTokenize(tempStr, fields, ",");
	if(fields.size() != 12)
	{
		if (logging)
			logfile << "Incorrect Number of Fields: " << tempStr << endl;
		printf("\nIncorrect Number of Fields, Ignoring Data\n");
		lines.clear();
		return;
	}

	if(fields[2].compare("A") == 0)
		update.validFix = true;
	else
		update.validFix = false;

	if(fields[3].length() < 3)
	{
		update.validFix = false;
		update.latitude = 0;
	}
	else
	{
		degrees = 10 * (fields[3][0] - '0') + fields[3][1] - '0';
		minutes = atof(fields[3].c_str() + 2);
		update.latitude = degrees + minutes / 60.0;
	}

	if(fields[4].compare("S") == 0)
		update.latitude *= -1;

	if(fields[5].length() < 4)
	{
		update.validFix = false;
		update.longitude = 0;
	}
	else
	{
		degrees = 100 * (fields[5][0] - '0') + 10 * (fields[5][1] - '0') + fields[5][2] - '0';
		minutes = atof(fields[5].c_str() + 3);
		update.longitude = degrees + minutes / 60.0;
	}

	if(fields[6].compare("W") == 0)
		update.longitude *= -1;
	
	update.speed = 0;
	update.speed = Speed::Kt_MPerSec(atof(fields[7].c_str()));
	update.heading = Angle::DEG_RAD(atof(fields[8].c_str()));

	printf("\rLat: %2.9f, Lon: %2.9f, Speed: %2.2f, Heading: %1.4f",update.latitude,update.longitude,update.speed,update.heading);

	if (logging) {
		char str[1000];
		sprintf(str, "Lat: %2.9f, Lon: %2.9f, Speed: %2.2f, Heading: %1.4f\n", update.latitude,update.longitude,update.speed,update.heading);
		logfile << str;	
	}

	rawUpdate = (RawGPSUpdateType)update;
	// Send the raw GPS
	Messages::RawGPSUpdate.publish(&rawUpdate);

	//Convert to meters
	Pave_Libraries_GPSTransforms::GeodeticPoint point(update.latitude,update.longitude);
	Pave_Libraries_GPSTransforms::CartesianLocalPoint& localPoint =	Pave_Libraries_GPSTransforms::GPSTransforms::ConvertToLocal(point);
	update.latitude = localPoint.X;
	update.longitude = localPoint.Y;

	Messages::GPSUpdate.publish(&update);

    lines.clear();
}


int main(int argc, char **argv)
{
	bool nowindow = false;
    if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (strcmp(argv[i], "-logging") == 0)
				logging = true;
			else if (strcmp(argv[i], "-nowindow") == 0)
				nowindow = true;
			else
				cout << "Usage: use -logging to log to text file, -nowindow to hide window" << endl;
		}
    }

	if (nowindow) {
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_HIDE );
	}

    //open file for logging
    if (logging) { 
        //logging
	    time_t theTime;
	    time(&theTime);
	    tm *localTime = localtime(&theTime);
	    char filename[1000];
	    sprintf_s(filename, 1000, LOGGING_ROOT_GPS);
	    sprintf_s(filename + strlen(LOGGING_ROOT_GPS), 1000, "GPS-%04d-%02d-%02d-%02d-%02d-%02d.csv", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
	    logfile.open(filename);
        logfile << "Latitude\tLongitude\tSpeed\tHeading\n";
    }


    cout << "Starting GPS service... " << endl;

	PortSettings settings;
	settings.BaudRate = BAUD57600;
	settings.DataBits = DATA_8;
	settings.FlowControl = FLOW_OFF;
	settings.Parity = PAR_NONE;
	settings.StopBits = STOP_1;
	settings.Timeout_Millisec = 500;

	bool success = false;
	char *port = Data::getString("GPS_SerialPort", success);
	if (!success) {
		cout << "Which COM port? Check configuration file" << endl;
		return 0;
	}

	serialPort = new QextSerialPort(port,settings);
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
	serialPort->open(QIODevice::ReadOnly);
    serialPort->setQueryMode(QextSerialBase::Polling);
	serialPort->flush();

	intervalTimer = new CPerformanceTimer();
    intervalTimer->Start();

	IPCConnect("GPS");
	if (!IPC_isConnected()) exit(0);
	IPC_setVerbosity(IPC_Print_Warnings);

    while (IPC_isConnected() > 0) {
        timerHandler();
        Sleep(1);
    }

    return 0;
}
