#include "GPSPositionAndHeadingMeasurementModel.h"

ReturnMatrix GPSPositionAndHeadingMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{
	ColumnVector Y(3); 
	
	Y(1) = x(1) + v(1);
	Y(2) = x(2) + v(2);
	Y(3) = x(3) + v(3);
	
	Y.release();
	return Y;
}