#pragma once

namespace Pave_Libraries_GPSTransforms
{
	class WGS84
	{
	public:
		static const double Flattening;
        static const double SemiMajorAxis;
        static const double SemiMinorAxis;
        // The eccentricity^2 should equal 0.0066943799901413800
        static const double EccentricitySquared;
        // The 2nd eccentricity squared should equal 0.00673949675658690300
        static const double SecondEccentricitySquared;
	};

	
}