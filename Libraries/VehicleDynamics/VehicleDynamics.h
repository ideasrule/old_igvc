#pragma once

#define WANT_STREAM                  // include.h will get stream fns
#define WANT_MATH                    // include.h will get math fns
#define WANT_FSTREAM
#define WANT_TIME                

#include "newmatap.h"                // need matrix applications
#include "newmatio.h"                // need matrix output routines

#ifdef use_namespace
using namespace NEWMAT;              // access NEWMAT namespace
#endif

namespace Pave_VehicleParameters {
	const double TrackWidth = 0.597;		    //TrackWidth of the robot in meters
	const double Length = 0.9398;		        //length of the robot, in meters
	const double ReferenceOffset = 0.6858 ;	//distance between reference point and front of robot, in meters
	const double Width = 0.7874 ;              // overall width of robot in meters    
	const double MaxWheelSpeed = 2.23;          //max wheel speed, in meters/second
	const double UpperLimitWheelSpeed = 5.0;	//absolute maximum wheel speed possible
	const double MaxYawRate = 2*MaxWheelSpeed / TrackWidth;
}

class VehicleDynamics
{
public:
	
	//x is a column vector consisting of [x y heading], v is noise [V omega]', and u is input [vr vl]'
	//returns predicted new x
	static ReturnMatrix Predict(const ColumnVector& x, const ColumnVector& v, const ColumnVector &u);
	//returns the max forward speed possible during a turn, given the desired yaw rate
	static double MaxFwdSpeedTurn(double yawRate);
	//returns a 2 item column vector [LeftSpeed; RightSpeed], given input vector (V,Omega)
	static ReturnMatrix WheelSpeeds(const ColumnVector& vOmega);
	
};