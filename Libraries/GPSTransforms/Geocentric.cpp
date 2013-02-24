#include "GeodeticPoint.h"
#include "GeocentricPoint.h"
#include "Geocentric.h"
#include "WGS84.h"
#include <math.h>
//#include "wykobi.hpp"

#define PI 3.1415926535

namespace Pave_Libraries_GPSTransforms
{
	GeocentricPoint Geocentric::GeodeticToGeocentric(const GeodeticPoint& InputPoint)
	{
		GeocentricPoint PointOut;
		double InLong = InputPoint.Longitude;
		if (InLong > 180.0)
			InLong -= 360.0;

		double LatitudeInRadians = InputPoint.LatitudeInRadians();
		double LongitudeInRadians = InLong * PI / 180.0;//InputPoint.LongitudeInRadians();

		double Lat_Cos = cos(LatitudeInRadians);
		double Lat_Sin = sin(LatitudeInRadians);

		//Radius of the earth at the specified location
		double EarthRadius = WGS84::SemiMajorAxis / (sqrt(1.0 - WGS84::EccentricitySquared * pow(Lat_Sin, 2.0)));
		double HeightFromCenter = EarthRadius + InputPoint.Height;

		// The values this function is returning differ by several tens
		// from the values returned by the Army GEOTRANS program
		PointOut.X = HeightFromCenter * Lat_Cos * cos(LongitudeInRadians);
		PointOut.Y = HeightFromCenter * Lat_Cos * sin(LongitudeInRadians);
		PointOut.Z = ((EarthRadius * (1 - WGS84::EccentricitySquared)) + InputPoint.Height) * Lat_Sin;

		return PointOut;
	}

	GeodeticPoint Geocentric::GeocentricToGeodetic(const GeocentricPoint& InputPoint)
	{
		// The method used here is derived from 'An Improved Algorithm for
		// Geocentric to Geodetic Coordinate Conversion', by Ralph Toms, Feb 1996

		GeodeticPoint OutputPoint;

		double W;           // Distance from Z axis
		//double W2;          // square of distance from Z axis
		double T0;          // Initial estimate of vertical component
		double T1;          // Corrected estimate of vertical component
		double S0;          // Initial estimate of horizontal component
		double S1;          // Corrected estimate of horizontal component

		//double B0;          // Estimate of Bowring aux variable
		double Sin_B0;      // sin(B0)
		//double Sin3_B0;     // cube of sin(B0)
		double Cos_B0;      // cos(B0)
		double Sin_p1;      // sin(phi1), phi1 is estimated latitude
		double Cos_p1;      // cos(phi1)
		double EarthRadius;          // Earth radius at location
		double Sum;         // numerator of cos(phi1)
		bool AtPole = false;        // Indicates whether location is in polar region

		//AtPole = false;

		if (InputPoint.X != 0.0)
			OutputPoint.Longitude = atan2(InputPoint.Y, InputPoint.X);
		else
		{
			if (InputPoint.Y > 0.0)
				OutputPoint.Longitude = PI / 2.0;

			else if (InputPoint.Y < 0)
				OutputPoint.Longitude = -PI / 2.0;

			else
			{
				AtPole = true;
				OutputPoint.Longitude = 0.0;
				if (InputPoint.Z > 0.0)
				{
					// North pole
					OutputPoint.Latitude = PI / 2.0;
				}
				else if (InputPoint.Z < 0.0)
				{
					// South pole
					OutputPoint.Latitude = -PI / 2.0;
				}
				else
				{
					// Center of earth
					OutputPoint.Latitude = PI / 2.0;
					OutputPoint.Height = -WGS84::SemiMinorAxis;
				}
			}
		}

		W = sqrt(pow(InputPoint.X, 2.0) + pow(InputPoint.Y, 2.0));

		T0 = InputPoint.Z * 1.0026000;         // Multiplies Z by "Toms region 1 constant"
		S0 = sqrt(pow(T0, 2.0) + pow(W, 2.0));
		Sin_B0 = T0 / S0;
		Cos_B0 = W / S0;
		T1 = InputPoint.Z + WGS84::SemiMinorAxis * WGS84::SecondEccentricitySquared * pow(Sin_B0, 3.0);
		Sum = W - WGS84::SemiMajorAxis * WGS84::EccentricitySquared * pow(Cos_B0, 3.0);
		S1 = sqrt(pow(T1, 2.0) + pow(Sum, 2.0));
		Sin_p1 = T1 / S1;
		Cos_p1 = Sum / S1;
		EarthRadius = WGS84::SemiMajorAxis / sqrt(1.0 - WGS84::EccentricitySquared * pow(Sin_p1, 2.0));


		if (Cos_p1 >= cos(67.5 * 90.0 / PI))
			OutputPoint.Height = W / Cos_p1 - EarthRadius;

		else if (Cos_p1 <= -cos(67.5 * 90.0 / PI))
			OutputPoint.Height = W / -Cos_p1 - EarthRadius;

		else
			OutputPoint.Height = InputPoint.Z / Sin_p1 + EarthRadius * (WGS84::EccentricitySquared - 1.0);

		if (AtPole == false)
			OutputPoint.Latitude = atan2(Sin_p1, Cos_p1);

		return OutputPoint;
	}
}