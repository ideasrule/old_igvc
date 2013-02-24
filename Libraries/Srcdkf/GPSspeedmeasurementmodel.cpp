#include "GPSspeedmeasurementmodel.h"

ReturnMatrix GPSSpeedMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{
	double vr = x(5);
	double vl = x(6);

	ColumnVector Y(1); 
	
	Y(1) = abs((vr+vl)/2.0 + v(1)) ;
	
	Y.release();
	return Y;
}