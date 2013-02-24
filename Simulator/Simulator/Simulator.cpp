// Simulator.cpp : Defines the entry point for the console application.
//

#include "Common.h"
#include "Utils.h"
#include "SimState.h"
#include "VehicleDynamics.h"

#include "newmatap.h"             
#include "newmatio.h"    

#include <math.h>
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cstring>

using namespace Pave_Libraries_Conversions;

#define INPUT_TICK 50

#define GPS_IN_METERS 10
#define PI 3.1415926535

SimState *simstate;
ofstream *logFile;


int ticknum = 0;
void SetSpeedHandler(void *data)
{
	WheelSpeedType *desiredSpeeds = (WheelSpeedType *)data;
	simstate->x(5) = desiredSpeeds->right;
	simstate->x(6) = desiredSpeeds->left;
	simstate->x(7) = (desiredSpeeds->right - desiredSpeeds->left) / Pave_VehicleParameters::TrackWidth;
	
}

void StateEstHandler(void *data)
{
	StateEstimationType *se = (StateEstimationType *)data;
	try
	{
		//*logFile << se->Easting << "," << se->Northing << "," << se->Heading << "," << se->Speed << ","  << 
		//	simstate->x(1) << "," << simstate->x(2) << "," << simstate->x(3) << "," << simstate->x(4) << "," << (simstate->x(5) + simstate->x(6))/2.0 << "," << simstate->x(5) << "," << simstate->x(6) << "," << simstate->x(7) << "," << simstate->lastGPS(1) << "," << simstate->lastGPS(2) << "," << simstate->lastGPS(3) << "," << simstate->lastGPS(4) << endl;
	}
	catch(...)
	{

	}
}
ReturnMatrix MultivariateGaussian(const LowerTriangularMatrix& sqrtCovariance)
{
	ColumnVector v(sqrtCovariance.ncols());

	for(int i = 1; i <= v.nrows(); i++)
		v(i) = Utils::Gaussian();

	v = sqrtCovariance*v;

	v.release();
	return v;

}

LARGE_INTEGER currentTick;
LARGE_INTEGER lastTick;
LARGE_INTEGER frequency;

void tick()
{
	ticknum++;

	// update time
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&currentTick);
	ColumnVector u(1); u = (double)(((long double)currentTick.QuadPart - (long double)lastTick.QuadPart)/(long double)frequency.QuadPart);
	lastTick = currentTick;

	if(u(1) > 0.5) //If this update takes forever, don't incorporate it
		u(1) = 0.05;
	

	simstate->x(5);
	WheelSpeedType ws;
	ws.right = simstate->x(5);
	ws.left = simstate->x(6);
	Messages::GetWheelSpeed.publish(&ws);

	ColumnVector v(4); v  = 0.0;//MultivariateGaussian(simstate->Sv); //Noise applied to V and Omega speeds

	//Predict state
	simstate->x = VehicleDynamics::Predict(simstate->x, v, u);
	simstate->x(3) = Utils::SquashAngle(simstate->x(3));
	
	//Issue GPS update
	if(ticknum == 1 || ticknum%2==0)
	{
		ColumnVector y(4); y = MultivariateGaussian(simstate->Sn);// = 0;
		GPSUpdateType gut;
		gut.latitude = simstate->x(1) + y(1);
		gut.longitude = simstate->x(2) + y(2);
		gut.heading = Utils::SquashAngle(simstate->x(3) + ((simstate->x(5) + simstate->x(6))<0?PI:0) + y(3));
		gut.speed = abs((simstate->x(5) + simstate->x(6))/2.0 + y(4)); 
		gut.validFix = GPS_IN_METERS; //Magic number that means it isn't actually lat/lon, it's in meters. necessary hack.
		
		simstate->lastGPS(1) = y(1) + simstate->x(1);
		simstate->lastGPS(2) = y(2) + simstate->x(2);
		simstate->lastGPS(3) = Utils::SquashAngle(y(3) + simstate->x(3));
		simstate->lastGPS(4) = y(4) + (simstate->x(5) + simstate->x(6))/2.0;

		Messages::GPSUpdate.publish(&gut);
	}

	//Issue compass update
	if(ticknum == 1 || ticknum%2==0)
	{
		double y = Utils::Gaussian() * .05;
		CompassUpdateType cut;
		cut.heading = Utils::SquashAngle(simstate->x(3) + simstate->x(4) + y);
		cut.pitch = 0;
		cut.roll = 0;

		Messages::CompassUpdate.publish(&cut);
	}

	//Issue gyro update
	if(ticknum == 1 || ticknum%2==0)
	{
		double y = Utils::Gaussian() * .01;
		GyroUpdateType gut;
		gut.yawRate = simstate->x(7) + y;

		Messages::GyroUpdate.publish(&gut);
	}

	cout << "x = " << setw(5) << setprecision(2) << fixed << right << simstate->x(1) 
		<< " y = " << setw(5) << setprecision(2) << fixed << right << simstate->x(2) 
		<< " theta = " << setw(5) << setprecision(2) << fixed << right << simstate->x(3) << '\n';
}

int main(int argc, char* argv[])
{

	//Connect to IPC
	IPC_connectModule("Simulator","localhost");

	try
	{
		//Open logging file with date as the file name
		time_t rawtime;
		char logFileName[100];
		time( &rawtime );
		//strftime(logFileName,100,"C:\\IGVC_2009\\Logs\\%m.%d.%y %I.%M.%S%p.txt", localtime( &rawtime ));
		strftime(logFileName,100,"%m.%d.%y %I.%M.%S%p.txt", localtime( &rawtime ));
		logFile = new ofstream(logFileName);

		//Initialize state and input
		simstate = new SimState();

		const double vrInit = 1.0;
		const double vlInit = 0.0;
		const double omegaInit = (vrInit - vlInit) / Pave_VehicleParameters::TrackWidth;
		simstate->x << 0 << 0 << 3.14 << Angle::DEG_RAD(-7.15) << vrInit << vlInit << omegaInit;
		
		simstate->Sv << .001					//delta noise
					 << 0		<< .6			//vr noise
					 << 0		<<	0 << .6;	//vl noise

	    simstate->Sn << .6 
					 << 0 << .6 
					 << 0 << 0 << .05
					 << 0 << 0 <<   0  <<.05;

		//Subscribe to GPS and wheel speed

		Messages::StateEstimation.subscribe(StateEstHandler,true,false);
		Messages::SetWheelSpeed.subscribe(SetSpeedHandler,true,false);

		Timer::addTimer(tick, INPUT_TICK);

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
	delete simstate;
	IPC_disconnect();
	return 0;
}


