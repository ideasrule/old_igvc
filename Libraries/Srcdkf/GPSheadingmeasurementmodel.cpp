#include "GPSheadingmeasurementmodel.h"
#define PI 3.14159265358979323

ReturnMatrix GPSHeadingMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{

	double theta = x(3);
	double vr =    x(5);
	double vl =    x(6);

	ColumnVector Y(1); 
	
	//See tech paper for an explanation. Corrects for the fact that heading can be off
	//by 180 deg if the robot is going in reverse
	Y(1) = theta - ((vr + vl)<0?PI:0) + v(1);

	Y.release();
	return Y;
}