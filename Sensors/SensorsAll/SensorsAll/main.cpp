#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include "Common.h"
#include "Messages.h"

#define LOGGING_ROOT_SENSORS_ALL "C:\\IGVC\\Logs\\"

using namespace std;
CPerformanceTimer *intervalTimer;
ofstream logfile;
using Pave_Libraries_Common::Messages;

double compassHeadingRaw = 0, leftSpeedRaw = 0, rightSpeedRaw = 0, yawRateRaw = 0;
double gpsLat = 0, gpsLong = 0, gpsNorthing, gpsEasting, gpsHeading = 0, gpsSpeed = 0;
double compassPitch = 0, compassRoll = 0;

void printStuff(void)
{
	time_t theTime;
	time(&theTime);
	logfile << fixed;
	ostream& os = logfile << setprecision(8);
	os /*<< theTime << "\t"*/
			<< gpsNorthing << "\t"
		    << gpsEasting << "\t"
			<< gpsHeading << "\t"
		    << gpsSpeed << "\t"
		    << gpsLat << "\t"
		    << gpsLong << "\t"
			<< leftSpeedRaw << "\t"
			<< rightSpeedRaw << "\t"
			<< yawRateRaw << "\t"
			<< compassHeadingRaw << "\t"
			<< compassPitch << "\t"
			<< compassRoll 
			<< std::endl;
}

void GPSHandler(void *data)
{
	try {
		const GPSUpdateType *d = (const GPSUpdateType *) data;

		gpsNorthing = d->latitude;
		gpsEasting = d->longitude;
		gpsHeading = d->heading;
		gpsSpeed = d->speed;
		printStuff();
	}
	catch( exception &e) { cerr << e.what() << endl; }
	catch(...)           { cerr << "non-exception error" << endl; }
}

void RawGPSHandler(void *data)
{
	try {
		const RawGPSUpdateType *d = (const RawGPSUpdateType *) data;

		gpsLat = d->latitude;
		gpsLong = d->longitude;
		printStuff();
	}
	catch( exception &e) { cerr << e.what() << endl; }
	catch(...)           { cerr << "non-exception error" << endl; }
}

void CompassHandler(void *data)
{
	try {
		const CompassUpdateType *d = (const CompassUpdateType *) data;
		
		compassHeadingRaw = d->heading;
		compassPitch = d->pitch;
		compassRoll = d->roll;
		printStuff();
	}
	catch( exception &e) { cerr << e.what() << endl; }
	catch(...)           { cerr << "non-exception error" << endl; }
}

void GyroHandler(void *data)
{
	try {
		const GyroUpdateType *d = (const GyroUpdateType *) data;

		yawRateRaw = d->yawRate;
		printStuff();
	}
	catch( exception &e) { cerr << e.what() << endl; }
	catch(...)           { cerr << "non-exception error" << endl; }
}

void WheelSpeedHandler(void *data)
{
	try {
		const WheelSpeedType *d = (const WheelSpeedType *) data;

		leftSpeedRaw = d->left;
		rightSpeedRaw = d->right;
		printStuff();
	}
	catch( exception &e) { cerr << e.what() << endl; }
	catch(...)           { cerr << "non-exception error" << endl; }
}

int main() {

    //logging
    time_t theTime;
    time(&theTime);
    tm *localTime = localtime(&theTime);
    char filename[1000];
    sprintf_s(filename, 1000, LOGGING_ROOT_SENSORS_ALL);
    sprintf_s(filename + strlen(LOGGING_ROOT_SENSORS_ALL), 1000, "SA-%04d-%02d-%02d-%02d-%02d-%02d.csv", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
    logfile.open(filename);
    logfile << "GPSNorthing\t\
				GPSEasting\t\
				GPSHeading\t\
				GPSSpeed\t\
				GPS Lat\t\
				GPS Long\t\
				Rightspeed\t\
				Leftspeed\t\
				Yawrate\t\
				Compass Heading\t\
				Compass Pitch\t\
				Compass Roll";

	//Connect to IPC
	IPC_connectModule("SensorsAll","localhost");

	Messages::GPSUpdate.subscribe(GPSHandler, true, false);
	Messages::RawGPSUpdate.subscribe(RawGPSHandler, true, false);
	Messages::CompassUpdate.subscribe(CompassHandler, true, false);
	Messages::GyroUpdate.subscribe(GyroHandler, true, false);
	Messages::GetWheelSpeed.subscribe(WheelSpeedHandler, true, false);
	//Messages::

	IPC_dispatch();
	return 0;
}