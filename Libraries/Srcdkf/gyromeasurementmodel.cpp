#include "gyromeasurementmodel.h"

ReturnMatrix GyroMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{
	ColumnVector Y(1); 
	
	Y(1) = x(7) + v(1);
	
	Y.release();
	return Y;
}