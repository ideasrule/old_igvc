#include "GPSpositionmeasurementmodel.h"

ReturnMatrix GPSPositionMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{
	ColumnVector Y(2); 
	
	Y(1) = x(1) + v(1);
	Y(2) = x(2) + v(2);
	
	Y.release();
	return Y;
}