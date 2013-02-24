#pragma once
#include "CartesianLocalPoint.h"
#include "GeodeticPoint.h"
#include "GeocentricPoint.h"

namespace Pave_Libraries_GPSTransforms
{
	class CartesianLocal
	{
	public:
		/// <summary>
        /// This function converts geocentric (earth-centered cartesian) to a local cartesian system, using the origin specified
        /// </summary>
        /// <param name="InputPoint">The point to be converted, geocentric XYZ in meters</param>
        /// <param name="LocalOrigin">The latitude, longitude and height of the point to be considered (0,0,0) 
        /// in our local system.</param>
        /// <returns>Returns the local coordinates of the latitude and longitude specified</returns>
		static CartesianLocalPoint GeodeticToLocal(const GeodeticPoint& InputPoint, const GeodeticPoint& LocalOrigin);

		/// <summary>
        /// This function converts geocentric (earth-centered cartesian) to a local cartesian system, using the origin specified
        /// </summary>
        /// <param name="InputPoint">The point to be converted, geocentric XYZ in meters</param>
        /// <param name="LocalOrigin">The latitude, longitude and height of the point to be considered (0,0,0) 
        /// in our local system.</param>
        /// <returns>Returns the local coordinates of the latitude and longitude specified</returns>
		static CartesianLocalPoint GeocentricToLocal(const GeocentricPoint& InputPoint, const GeodeticPoint& LocalOrigin);

		static GeocentricPoint LocalToGeocentric(const CartesianLocalPoint& InputPoint, const GeodeticPoint& LocalOrigin);
	};
}