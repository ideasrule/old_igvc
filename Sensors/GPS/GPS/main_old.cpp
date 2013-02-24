#define _WIN32_WINNT 0x0500
#include "ipc.h"
#include "Common.h"
#include "GPSTransforms.h"
#include "qextserialport.h"
#include <ctime>
#include <cstdio>
#include <string>
#include <vector>
#include <windows.h>
#include <iostream>
#include <fstream>

#define LOGGING
#define LOGGING_ROOT_GPS "C:\\IGVC\\Logs\\"

using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Conversions;

using std::cout;
using std::vector;

//these are commented out because we use the conversions library
//#define MS_PER_KNOT .514444444
//#define PI 3.141592653589793238462643383279500
//#define PI_DIV_180 0.017453292519943295769236907684886

#define BUFFER_LENGTH 5000
#define TICK_RATE 25

#define MAX_MESSAGE_SIZE 500

string parseSerialPortError(ulong errorCode);

QextSerialPort serialPort("COM4");
string megaBuffer;
vector<string> lines;
vector<string> fields;
vector<string> location;
GPSUpdateType update;
char checksum;
char checksumString[20];
double degrees, minutes;

char buffer[BUFFER_LENGTH + 1];
long readLength;

LARGE_INTEGER frequency;
LARGE_INTEGER lastTick;
LARGE_INTEGER now;

ofstream myFile;

BOOL CtrlHandler( DWORD fdwCtrlType ) 
{
	if(serialPort.isOpen())
		serialPort.close();
	myFile.close();
	return FALSE;
}


void timerHandler()
{

	if(!serialPort.isOpen())
	{
	#ifdef LOGGING
		//myFile << "Serial Port Not Open!" << endl;
	#endif
		printf("Serial Port Not Open!\n");
		return;
	}

	while(serialPort.size() > 0)
	{
		readLength = serialPort.readLine(buffer, BUFFER_LENGTH);
		if(readLength <= 0)
			break;
		for(int i = 0; i < readLength; i++)
		{
			if(buffer[i] == '\0')
			{
				//megaBuffer.clear();
				//return;
				buffer[i] = '*';
				//cout << "Blowing off 0 in buffer\n";
			}
		}
		buffer[readLength] = '\0';
		megaBuffer += buffer;
	}

	//if(megaBuffer.length() >= 2*MAX_MESSAGE_SIZE)
		//megaBuffer = megaBuffer.substr(megaBuffer.length() - 2*MAX_MESSAGE_SIZE, 2*MAX_MESSAGE_SIZE);
	Data::stringTokenize(megaBuffer, lines, "\r\n");
	//cout << "\nNumber of lines: " << lines.size() << endl;
	for(int i = 0; i < lines.size(); i++)
	{
#ifdef LOGGING
		//myFile << "Line: " << lines[i] << endl;
#endif
		//printf("\nLine: %s\n", lines[i].c_str());
		int start;
		if((start = lines[i].find("GPRMC")) >= 0)
		{
			// Check the checksum
			int checkSumIndex = lines[i].find("*");
			if(checkSumIndex < 0 || checkSumIndex + 2 >= lines[i].length())
				continue;
			for(int j = start; j < lines[i].length() && lines[i][j] != '*'; j++)
			{
				if(j == start)
					checksum = lines[i][j];
				else
					checksum ^= lines[i][j];
			}
			if(checksum >= 16)
				sprintf(checksumString, "%X", checksum);
			else
				sprintf(checksumString, "0%X", checksum);
			bool noGood = false;
			for(int j = 0; j < 2; j++)
			{
				if(*(lines[i].c_str() + checkSumIndex + 1 + j) == '*'&& checksumString[j] != '\0')
				{
					noGood = true;
					break;
				}
				else if(*(lines[i].c_str() + checkSumIndex + 1 + j) != checksumString[j])
				{
					noGood = true;
					break;
				}
			}
			if(noGood)
			{
				#ifdef LOGGING
					//myFile << "Invalid Checksum: " << lines[i] << endl;
				#endif
				printf("\nInvalid Checksum, Ignoring Data\n");
				continue;
			}

			// If it's ok, parse and send
			Data::stringTokenize(lines[i], fields, ",");
			if(fields.size() != 13)
			{
				#ifdef LOGGING
					//myFile << "Incorrect Number of Fields: " << lines[i] << endl;
				#endif
				printf("\nIncorrect Number of Fields, Ignoring Data\n");
				continue;
			}

			if(fields[2].compare("A") == 0)
				update.validFix = true;
			else
				update.validFix = false;

			//update.latitude = atof(fields[3].c_str());
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

			//update.longitude = atof(fields[5].c_str()) * .01;
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

#ifdef LOGGING
			//myFile << "Lat: " << update.latitude << ", Lon: " << update.longitude << ", Speed: " << update.speed 
			//	<< ", Heading: " << update.heading << endl;
			//fprintf(myFile, "abc");
			char str[1000];
			sprintf(str, "Lat: %2.9f, Lon: %2.9f, Speed: %2.2f, Heading: %1.4f\n", update.latitude,update.longitude,update.speed,update.heading);
			myFile << str;
#endif
			printf("\rLat: %2.9f, Lon: %2.9f, Speed: %2.2f, Heading: %1.4f",update.latitude,update.longitude,update.speed,update.heading);

			// Send the raw GPS
			Messages::RawGPSUpdate.publish((RawGPSUpdateType*)&update);

			//Convert to meters
			Pave_Libraries_GPSTransforms::GeodeticPoint point(update.latitude,update.longitude);
			Pave_Libraries_GPSTransforms::CartesianLocalPoint& localPoint =	Pave_Libraries_GPSTransforms::GPSTransforms::ConvertToLocal(point);
			update.latitude = localPoint.X;
			update.longitude = localPoint.Y;


			Messages::GPSUpdate.publish(&update);

			QueryPerformanceCounter(&now);
			double elapsed = (double)(now.QuadPart - lastTick.QuadPart)/(frequency.QuadPart);
			if(elapsed * 1000 > 5*TICK_RATE)
				printf("\nLong Cycle Time: %f ms\n", elapsed * 1000);
			//myFile << elapsed << endl;
			lastTick = now;

			// Clear the buffer, get out of here
			megaBuffer.clear();
			return;
		}
		else
		{
#ifdef LOGGING
			//myFile << "No GPRMC header found: " << lines[i] << endl;
#endif
			//cout << "\nNo GPRMC header found: " << parseSerialPortError(serialPort.lastError()) << endl;

			QueryPerformanceCounter(&now);
			double elapsed = (double)(now.QuadPart - lastTick.QuadPart)/(frequency.QuadPart);
			if(elapsed * 1000 > 20*TICK_RATE)
			{
				cout << "Fucked, resetting.\n";
				cout << "Buffer : " << megaBuffer;
				serialPort.flush();
				megaBuffer.clear();
				serialPort.close();
				serialPort.open(QIODevice::ReadOnly);
			}
		}
	}

}

int main(int argc, char **argv)
{
	bool nowindow = false;
	bool loggingTemp;
    if (argc > 1) {
		for (int i = 1; i < argc; ++i) {
			if (strcmp(argv[i], "-logging") == 0)
				loggingTemp = true;
			else if (strcmp(argv[i], "-nowindow") == 0)
				nowindow = true;
			else
				cout << "Usage: use /*-logging to log to text file*/, -nowindow to hide window" << endl;
		}
    }

	if (nowindow) {
		HWND hWnd = GetConsoleWindow();
		ShowWindow( hWnd, SW_HIDE );
	}

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&lastTick);
	SetConsoleCtrlHandler( (PHANDLER_ROUTINE) CtrlHandler, TRUE );
#ifdef LOGGING
	//logging
	time_t theTime;
	time(&theTime);
	tm *localTime = localtime(&theTime);
	char filename[1000];
	sprintf(filename, LOGGING_ROOT_GPS);
	sprintf(filename + strlen(LOGGING_ROOT_GPS), "%04d-%02d-%02d-%02d-%02d-%02d-GPS.txt", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
	myFile.open(filename);
#endif

	printf("Opening Serial Port\n");
	serialPort.setBaudRate(BaudRateType::BAUD57600);
	serialPort.open(QIODevice::ReadOnly);
	serialPort.flush();
	printf("Connecting to IPC\n");
	IPC_connectModule("GPS", "localhost");
	IPC_setVerbosity(IPC_VERBOSITY_TYPE::IPC_Print_Warnings);
	Timer::addTimer(&timerHandler, TICK_RATE);
	IPC_dispatch();
	myFile.close();
}

string parseSerialPortError(ulong errorCode)
{
	switch(errorCode)
	{
		case 0:
			return "No Error";
			break;
		case 1: 
			return "Invalid File Descriptor";
			break;
		case 2:
			return "Unable to Allocate Memory";
			break;
		case 3:
			return "Non Blocked Signal";
			break;
		case 4:
			return "Port Timeout";
			break;
		case 5:
			return "Invalid Device";
			break;
		case 6:
			return "Break Condition Detected";
			break;
		case 7: 
			return "Framing Error";
			break;
		case 8:
			return "IO Error";
			break;
		case 9: 
			return "Buffer Overrun";
			break;
		case 10:
			return "Receive Overflow";
			break;
		case 11:
			return "Parity Error";
			break;
		case 12:
			return "Transmit Overflow";
			break;
		case 13: 
			return "Read Failed";
			break;
		case 14:
			return "Write Failed";
			break;
		default:
			return "No Error";
			break;
	}
}