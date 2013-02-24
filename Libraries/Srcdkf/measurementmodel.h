#pragma once
#include "newmatap.h"                // need matrix applications

class MeasurementModel
{
public:
	//x is state, v is noise, u is input
	virtual ReturnMatrix measure(const ColumnVector& x, const ColumnVector v, const ColumnVector &u) = 0;
};