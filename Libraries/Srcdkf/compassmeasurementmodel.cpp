#include "compassmeasurementmodel.h"

ReturnMatrix CompassMeasurementModel::measure(const ColumnVector &x, const ColumnVector v, const ColumnVector &u)
{
	double theta = x(3);

	ColumnVector Y(1); 
	
	Y(1) = theta + v(1);
	
	Y.release();
	return Y;
}