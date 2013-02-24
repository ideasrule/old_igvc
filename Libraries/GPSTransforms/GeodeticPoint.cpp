#include "GeodeticPoint.h"
//#include "wykobi.hpp"
#define PI 3.1415926535
namespace Pave_Libraries_GPSTransforms
{
	GeodeticPoint::GeodeticPoint()
	{
	}

	GeodeticPoint::GeodeticPoint(double latIn, double longIn)
	{
		this->Latitude = latIn;
		this->Longitude = longIn;
		this->Height = 0;
	}

	double GeodeticPoint::LatitudeInRadians() const
	{
		return Latitude * PI / 180;
	}

	double GeodeticPoint::LongitudeInRadians() const
	{
		return Longitude * PI / 180;
	}
}