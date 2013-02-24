#include "VehicleDynamics.h"

#include "newmatap.h"                // need matrix applications
#include "newmatio.h"                // need matrix output routines
#include "Conversions.h"

using namespace Pave_Libraries_Conversions;
using namespace Pave_VehicleParameters;


ReturnMatrix VehicleDynamics::Predict(const ColumnVector& x, const ColumnVector& v, const ColumnVector &u)
{

	double dt =	u(1);
	double V = u(2);
	double omega = u(3);

	double xpos =		x(1);
	double ypos =		x(2);
	double theta =		x(3);

	double xNoise = v(1);
	double yNoise =	v(2);
	double thetaNoise =	v(3);

	ColumnVector chix(3); 
	
	//From Larsen, Hansen, Andersen, and Ravn, "Design of Kalman Filters for Mobile Robots," 1999
	chix(1) = xpos + dt*V*sin(theta + dt*omega/2.0) + xNoise;
	chix(2) = ypos + dt*V*cos(theta + dt*omega/2.0) + yNoise;
	chix(3) = theta + dt*omega + thetaNoise;
	
	chix.release();
	return chix;
}



ReturnMatrix VehicleDynamics::WheelSpeeds(const ColumnVector& vOmega)
{
	ColumnVector speeds(2);
	speeds(1) = vOmega(1) + (vOmega(2)*TrackWidth)/2.0; //signs may need to be flipped
	speeds(2) = vOmega(1) - (vOmega(2)*TrackWidth)/2.0;
	speeds.release();
	return speeds;
}

double VehicleDynamics::MaxFwdSpeedTurn(double yawRate)
	{
		return MaxWheelSpeed - abs((yawRate * TrackWidth) / 2.0);
	}
