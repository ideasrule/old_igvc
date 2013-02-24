#include "CartesianLocalPoint.h"

namespace Pave_Libraries_GPSTransforms
{
	CartesianLocalPoint::CartesianLocalPoint()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	CartesianLocalPoint::CartesianLocalPoint(double x, double y, double z)
	{
		X = x;
		Y = y;
		Z = z;
	}
}