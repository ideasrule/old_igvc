#pragma once

namespace Pave_Libraries_GPSTransforms
{
	class GeocentricPoint
	{
	public:
		GeocentricPoint();
		GeocentricPoint(double x, double y, double z);
		double X;
		double Y;
		double Z;
	};
}