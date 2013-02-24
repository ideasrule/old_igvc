#include "GeocentricPoint.h"

namespace Pave_Libraries_GPSTransforms
{
	GeocentricPoint::GeocentricPoint() { }

	GeocentricPoint::GeocentricPoint(double x, double y, double z)
	{
		X = x;
		Y = y;
		Z = z;
	}
}