#include "wheelencodermeasurementmodel.h"

ReturnMatrix WheelEncoderMeasurementModel::measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u)
{
	ColumnVector Y(2); 
	
	Y(1) = x(5) + v(1);
	Y(2) = x(6) + v(2);
	
	Y.release();
	return Y;
}