#pragma once

namespace Pave_Libraries_GPSTransforms
{
	class GeodeticPoint
	{
	public:
		GeodeticPoint();
		GeodeticPoint(double latIn, double longIn);

		double Latitude;
		double Longitude;
		double Height;

		double LatitudeInRadians() const;
		double LongitudeInRadians() const;
	};
}