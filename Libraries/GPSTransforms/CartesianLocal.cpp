#include "CartesianLocal.h"
#include "GeodeticPoint.h"
#include "CartesianLocalPoint.h"
#include "GeocentricPoint.h"
#include "Geocentric.h"
#include "WGS84.h"
#include "GeodeticPoint.h"
#include "CartesianLocalPoint.h"
#include "Geocentric.h"
#include "GeocentricPoint.h"
#include <math.h>

namespace Pave_Libraries_GPSTransforms
{
	CartesianLocalPoint CartesianLocal::GeodeticToLocal(const GeodeticPoint& InputPoint, const GeodeticPoint& LocalOrigin)
	{
		GeocentricPoint DummyGeocent = Geocentric::GeodeticToGeocentric(InputPoint);
		CartesianLocalPoint local = GeocentricToLocal(DummyGeocent, LocalOrigin);
		return local;
	}

	CartesianLocalPoint CartesianLocal::GeocentricToLocal(const GeocentricPoint& InputPoint, const GeodeticPoint& LocalOrigin)
	{
		CartesianLocalPoint OutputPoint;

		double Cos_LocalOriginLat = cos(LocalOrigin.LatitudeInRadians());
		double Sin_LocalOriginLat = sin(LocalOrigin.LatitudeInRadians());
		double Cos_LocalOriginLon = cos(LocalOrigin.LongitudeInRadians());
		double Sin_LocalOriginLon = sin(LocalOrigin.LongitudeInRadians());

		double N0 = WGS84::SemiMajorAxis / sqrt(1 - WGS84::EccentricitySquared * pow(Sin_LocalOriginLat, 2.0));
		double val = (N0 + LocalOrigin.Height) * cos(LocalOrigin.LatitudeInRadians());

		// These are the coordinates of the geocentric origin, 
		// in terms of the local cartesian coords

		double u0 = val * cos(LocalOrigin.LongitudeInRadians());
		double v0 = val * sin(LocalOrigin.LongitudeInRadians());
		double w0 = ((N0 * (1 - WGS84::EccentricitySquared)) + LocalOrigin.Height) * sin(LocalOrigin.LatitudeInRadians());
		double u_MINUS_u0 = InputPoint.X - u0;
		double v_MINUS_v0 = InputPoint.Y - v0;
		double w_MINUS_w0 = InputPoint.Z - w0;

		OutputPoint.X = -Sin_LocalOriginLon * u_MINUS_u0 + Cos_LocalOriginLon * v_MINUS_v0;
		OutputPoint.Y = -Sin_LocalOriginLat * (Cos_LocalOriginLon * u_MINUS_u0) + -Sin_LocalOriginLat * (Sin_LocalOriginLon * v_MINUS_v0) + Cos_LocalOriginLat * w_MINUS_w0;
		OutputPoint.Z = Cos_LocalOriginLat * (Cos_LocalOriginLon * u_MINUS_u0) + Cos_LocalOriginLat * (Sin_LocalOriginLon * v_MINUS_v0) + Sin_LocalOriginLat * w_MINUS_w0;

		return OutputPoint;
	}

	GeocentricPoint CartesianLocal::LocalToGeocentric(const CartesianLocalPoint& InputPoint, const GeodeticPoint& LocalOrigin)
	{
		GeocentricPoint OutputPoint;

		
		double Cos_LocalOriginLat = cos(LocalOrigin.Latitude);
		double Sin_LocalOriginLat = sin(LocalOrigin.Latitude);
		double Cos_LocalOriginLon = cos(LocalOrigin.Longitude);
		double Sin_LocalOriginLon = sin(LocalOrigin.Longitude);
		
		double N0 = WGS84::SemiMajorAxis / sqrt(1 - WGS84::EccentricitySquared * pow(Sin_LocalOriginLat, 2.0));
		double val = (N0 + LocalOrigin.Height) * cos(LocalOrigin.LatitudeInRadians());

		// These are the coordinates of the geocentric origin, 
		// in terms of the local cartesian coords
		double u0 = val * cos(LocalOrigin.LongitudeInRadians());
		double v0 = val * sin(LocalOrigin.LongitudeInRadians());
		double w0 = ((N0 * (1 - WGS84::EccentricitySquared)) + LocalOrigin.Height) * sin(LocalOrigin.LatitudeInRadians());

		double sin_lat_y = Sin_LocalOriginLat * InputPoint.Y;
		double cos_lat_z = Cos_LocalOriginLat * InputPoint.Z;

		OutputPoint.X = -Sin_LocalOriginLon * InputPoint.X - sin_lat_y * Cos_LocalOriginLon + cos_lat_z * Cos_LocalOriginLon + u0;
		OutputPoint.Y = Cos_LocalOriginLon * InputPoint.X - sin_lat_y * Sin_LocalOriginLon + cos_lat_z * Sin_LocalOriginLon + v0;
		OutputPoint.Z = Cos_LocalOriginLat * InputPoint.Y + Sin_LocalOriginLat * InputPoint.Z + w0;

		return OutputPoint;
	}
}