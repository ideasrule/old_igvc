#pragma once
#include "CartesianLocal.h"
#include "CartesianLocalPoint.h"
#include "Geocentric.h"
#include "GeocentricPoint.h"
#include "GeodeticPoint.h"
#include "WGS84.h"

#define ORIGIN_LATITUDE 42.67826383//42.67826383 <-oakland uni //40.349414<-princeton
#define ORIGIN_LONGITUDE -83.19545838//-83.19545838 <-oakland uni //-74.650630<-princeton

namespace Pave_Libraries_GPSTransforms
{
	class GPSTransforms
	{
	public:
		static CartesianLocalPoint ConvertToLocal(const GeodeticPoint& point);
	private:
		static const GeodeticPoint &localOrigin;
	};
}