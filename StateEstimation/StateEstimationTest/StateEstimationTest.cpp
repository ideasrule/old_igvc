// StateEstimationTest.cpp : Defines the entry point for the console application.
//

//logging
#include <time.h>
#include <fstream>
#include <iostream>
#define LOGGING_ROOT "C:\\IGVC_2009\\logs\\"
std::ofstream file;

#include "Common.h"

#include <iostream>

#define INPUT_TICK 100

double gpsHeading;
double compassHeading;
double compassYawRate;
double yawRate;
double gpsSpeed;
double leftWheelSpeed;
double rightWheelSpeed;

LARGE_INTEGER previousTick;
LARGE_INTEGER currentTick;
LARGE_INTEGER frequency;


void StateEstHandler(void *data)
{
	StateEstimationType *se = (StateEstimationType *)data;
	cout << se->Easting << "," << se->Northing << "," << se->Heading << endl;
	
}

void GPSHandler(void *data)
{
	GPSUpdateType *gps = (GPSUpdateType *)data;
	gpsHeading = gps->heading;
	gpsSpeed = gps->speed;
} 

void CompassHandler(void *data)
{
	QueryPerformanceCounter(&currentTick);
	double elapsed = (double)(((long double)currentTick.QuadPart - (long double)previousTick.QuadPart)/(long double)frequency.QuadPart);
	previousTick = currentTick;

	CompassUpdateType *cu = (CompassUpdateType *)data;
	compassYawRate = (cu->heading - compassHeading)/(elapsed);
	compassHeading = cu->heading;
}

void GyroHandler(void *data)
{
	GyroUpdateType *gu = (GyroUpdateType *)data;
	yawRate = gu->yawRate;
}
void WheelSpeedHandler(void *data)
{
	WheelSpeedType *wsu = (WheelSpeedType *)data;
	leftWheelSpeed = wsu->left;
	rightWheelSpeed = wsu->right;
}
void tick()
{

	printf("\rGPS_S: %4.2f, GPS_H: % 5.4f, Comp_H: %+5.4f, Gyro_Yaw: %+6.4f, Comp_Yaw: %+6.4f",gpsSpeed,gpsHeading,compassHeading,yawRate,compassYawRate);
	file << gpsSpeed << "," << gpsHeading << "," << compassHeading << "," << compassYawRate << "," << yawRate << "," << leftWheelSpeed << "," << rightWheelSpeed << endl;
}

int main(int argc, char* argv[])
{
	gpsHeading = 0;
	compassHeading = 0;
	compassYawRate = 0;
	yawRate = 0;
	gpsSpeed = 0;
	leftWheelSpeed = 0;
	rightWheelSpeed = 0;

	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentTick);

	//Connect to IPC
	IPC_connectModule("StateEstimationTest","localhost");

	time_t theTime;
	time(&theTime);
	tm *localTime = localtime(&theTime);
	char filename[1000];
	sprintf(filename, LOGGING_ROOT);
	sprintf(filename + strlen(LOGGING_ROOT), "%04d-%02d-%02d-%02d-%02d-%02d-SEtest.csv", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
	file.open(filename);
	file << "GPS Speed,GPS Heading,Compass Heading,Compass Yaw Rate,Gyro Yaw Rate,LeftWheel,RightWheel" << endl;

	try
	{
		//Subscribe to GPS and wheel speed

		Messages::StateEstimation.subscribe(StateEstHandler,true,false);
		Messages::GPSUpdate.subscribe(GPSHandler,true,false);
		Messages::CompassUpdate.subscribe(CompassHandler,true,false);
		Messages::GyroUpdate.subscribe(GyroHandler,true,false);
		Messages::GetWheelSpeed.subscribe(WheelSpeedHandler,true,false);

		Timer::addTimer(tick, INPUT_TICK);

		IPC_dispatch();
	}
	catch( exception &e)
	{
		cout << e.what() << endl;
	}
	catch(...)
	{
		cout << "non-exception error" << endl;
	}

	// teardown
	IPC_disconnect();
	return 0;
}


