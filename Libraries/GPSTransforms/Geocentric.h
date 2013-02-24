#pragma once
#include "GeocentricPoint.h"
#include "GeodeticPoint.h"

namespace Pave_Libraries_GPSTransforms
{
	class Geocentric
	{
	public:
		/// <summary>
        /// This function converts a point in geodetic coords (Lat, Lon, Height in meters) to geocentric coords (X, Y, Z in meters)
        /// </summary>
        /// <param name="InputPoint">The input point, in geodetic coordinates</param>
        /// <returns>A point in geocentric coordinates: X, Y, and Z in meters</returns>
		static GeocentricPoint GeodeticToGeocentric(const GeodeticPoint& InputPoint);
		static GeodeticPoint GeocentricToGeodetic(const GeocentricPoint& InputPoint);
	};
}