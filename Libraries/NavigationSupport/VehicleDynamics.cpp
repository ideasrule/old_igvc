#include "VehicleDynamics.h"

//#include "newmatap.h"                // need matrix applications
//#include "newmatio.h"                // need matrix output routines


namespace Pave_Libraries_Navigation
{
const double VehicleDynamics::Width = .8128;
const double VehicleDynamics::Length = 1.1938;
const double VehicleDynamics::ReferenceOffset = .7747;//.3937;
const double VehicleDynamics::Wheelbase = .6604; //m


//ReturnMatrix VehicleDynamics::Predict(const ColumnVector& x, const & ColumnVector& v, const ColumnVector &u)
//{
//
//	const float dt = .05;
//
//	double wheelbase =  .6604; //distance between center of wheels in m
//
//	double x =			x(1);
//	double y =			x(2);
//	double theta =		x(3);
//	double vnoise =		v(1);
//	double omeganoise = v(2);
//
//	double vr =			u(1);
//	double vl =			u(2);
//
//	double V = (vr + vl) / 2.0 + vnoise; //Translational displacement with error
//	double Omega = (vr - vl) / Wheelbase + omeganoise; //Rotational displacement with error
//
//
//	ColumnVector chix(3); //x, y, heading
//	
//	//From Larsen, Hansen, Andersen, and Ravn, "Design of Kalman Filters for Mobile Robots," 1999
//	chix(1) = x + dt*V*cos(theta + dt*Omega/2.0);
//	chix(2) = y + dt*V*sin(theta + dt*Omega/2.0);
//	chix(3) = theta + dt*Omega;//addangle(theta, dt*Omega);
//	
//	chix.release();
//	return chix;
//}
}