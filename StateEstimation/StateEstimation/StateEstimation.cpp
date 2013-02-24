// StateEstimation.cpp : Defies the entry point for the State Estimation service.
// I'm not too sure if this is the way it's supposed to work, 
// The basic state is [x, y, theta], and control is take to be velocity and angular velocity

#define _WIN32_WINNT 0x0500    //to use GetConsoleWindow() function
#include "Common.h"
#include "srcdkf.h"
#include "GPSTransforms.h"
#include "VehicleDynamics.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <windows.h>

#define GPS_IN_METERS 10 //Hack so that simulator can produce gps measurements in meters, not lat lon

// logging
bool logging = false;
#define LOGGING_ROOT_SE "C:\\IGVC\\Logs\\"
std::ofstream logfile;

Srcdkf *srcdkf = NULL;
CPerformanceTimer *intervalTimer;

int neverHadFix = 1;

double compassHeadingRaw = 0, leftSpeedRaw = 0, rightSpeedRaw = 0, yawRateRaw = 0;
double gpsLat = 0, gpsLong = 0, gpsHeading = 0, gpsSpeed = 0;
double updateTime = 0;

void PublishState()
{
	ColumnVector x = srcdkf->getState();
	//x now contains updated state

	StateEstimationType seType;
	seType.Easting = x(1);
	seType.Northing = x(2); 
	seType.Heading = x(3);
	seType.Speed = gpsSpeed;

	Messages::StateEstimation.publish(&seType);

	if (logging) {
		logfile << updateTime << '\t' << seType.Northing << '\t' << seType.Easting << '\t' << seType.Heading << '\t'
			<< seType.Speed << '\t' 
			<< gpsLat << '\t' << gpsLong << '\t' << gpsHeading << '\t' << gpsSpeed << '\t' << compassHeadingRaw << '\t' 
			<< yawRateRaw << endl;
	}

	cout << setw(15) << seType.Northing << '\t'
	     << setw(15) << seType.Easting << '\t'
	     << setw(15) << seType.Heading << '\t'
	     << setw(15) << seType.Speed << endl;
}


void GPSHandler(void *data)
{
	intervalTimer->Stop();
	updateTime = intervalTimer->Interval_S();
	intervalTimer->Start();
	ColumnVector u(3); u << updateTime << gpsSpeed << yawRateRaw;

	try
	{
		GPSUpdateType *gps = (GPSUpdateType *)data;
		
		if(gps->validFix || gps->validFix == GPS_IN_METERS)
		{
			ColumnVector y(3);
			//if(gps->validFix != GPS_IN_METERS)
			//{
			//	/*Pave_Libraries_GPSTransforms::GeodeticPoint point(gps->latitude,gps->longitude);
			//	Pave_Libraries_GPSTransforms::CartesianLocalPoint& localPoint =	Pave_Libraries_GPSTransforms::GPSTransforms::ConvertToLocal(point);*/

			//	y(1) = gps->latitude; //localPoint.X;
			//	y(2) = gps->longitude; //localPoint.Y;
			//	y(3) = gps->heading;
			//	y(4) = gps->speed;
			//}
			//else
			//{
			//	y(1) = gps->latitude; 
			//	y(2) = gps->longitude;
			//	y(3) = gps->heading;
			//	y(4) = gps->speed;
			//}

			y(1) = gps->latitude; 
			y(2) = gps->longitude;
			y(3) = gps->heading;     

			gpsLat = gps->latitude;
			gpsLong = gps->longitude;
			gpsHeading = gps->heading;
			gpsSpeed = gps->speed;

			if(neverHadFix) //First time we get GPS, just load position straight in
			{
				neverHadFix = 0;
				
				ColumnVector x = srcdkf->getState();
				
				x(1) = y(1);
				x(2) = y(2);
				if (gps->speed > 1.0) {
					x(3) = y(3);
				} 
				srcdkf->setState(x);
			}
			else
			{
				// do prediction step
				srcdkf->predict(u);   //u contains dt, speed, yawrate
				srcdkf->putAnglesInRange();

				if (gps->speed < 1.0) {
					srcdkf->GPSMeasurementNoHeading(y);
				} else {
					srcdkf->GPSMeasurementWithHeading(y);
				}
				PublishState();
			}
		}
	}
	catch( exception &e)
	{
		cout << e.what() << endl;
	}
	catch(BaseException)
	{
		cout << BaseException::what() << endl;
	}
	catch(...)
	{
		cout << "non-exception error" << endl;
	}
}



bool neverHadCompass = true;
void CompassHandler(void *data)
{
	intervalTimer->Stop();
	updateTime = intervalTimer->Interval_S();
	intervalTimer->Start();
	ColumnVector u(3); u = updateTime;

	try
	{
		CompassUpdateType *cut = (CompassUpdateType *)data;

		double cHeading = cut->heading;
		compassHeadingRaw = cHeading;
		ColumnVector y(1); y << cHeading;

		if(neverHadCompass)
		{
			neverHadCompass = false;
			
			ColumnVector x = srcdkf->getState();
			x(3) = y(1);
			srcdkf->setState(x);
		}
		else
		{
			// do prediction step
			srcdkf->predict(u);   //u contains dt, speed, yawrate
			srcdkf->putAnglesInRange();
			
			//Incorporate new heading measurement
			srcdkf->CompassMeasurement(y);

			PublishState();
		}
	}
	catch(exception &e)
	{
		cout << e.what() << endl;
	}
	catch(BaseException)
	{
		cout << BaseException::what() << endl;
	}
	catch(...)
	{
		cout << "non-exception error" << endl;
	}
}

bool neverHadGyro = true;
void GyroHandler(void *data)
{
	GyroUpdateType *gut = (GyroUpdateType *)data;
	yawRateRaw = gut->yawRate;
}


int main(int argc, char* argv[])
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
	    sprintf_s(filename, 1000, LOGGING_ROOT_SE);
	    sprintf_s(filename + strlen(LOGGING_ROOT_SE), 1000, "SE-%04d-%02d-%02d-%02d-%02d-%02d.csv", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
	    logfile.open(filename);
        logfile << "UpdateTime\tNorthing\tEasting\tHeading\tSpeed\tRightspeed\tLeftspeed\tGPS Lat\tGPS Long\tGPS Heading\tGPS Speed\tCompass Heading\tYaw Rate\n";
    }

	intervalTimer = new CPerformanceTimer();
    intervalTimer->Start();

	srcdkf = new Srcdkf();

	//Connect to IPC
	IPC_connectModule("StateEstimation","localhost");

	try
	{
		Messages::GPSUpdate.subscribe(GPSHandler, true, false);
		Messages::CompassUpdate.subscribe(CompassHandler, true, false);
		Messages::GyroUpdate.subscribe(GyroHandler, true, false);

		IPC_dispatch();
	}
	catch( exception &e)
	{
		cout << e.what() << endl;
	}
	catch(BaseException)
	{
		cout << BaseException::what() << endl;
	}
	catch(...)
	{
		cout << "non-exception error" << endl;
	}

	// teardown
	delete srcdkf;
	IPC_disconnect();
	logfile.close();
	return 0;
}

