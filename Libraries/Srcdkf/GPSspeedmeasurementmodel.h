#pragma once

#include "measurementmodel.h"

class GPSSpeedMeasurementModel : public MeasurementModel
{
	//x is state, v is noise, u is input
	ReturnMatrix measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u);
};