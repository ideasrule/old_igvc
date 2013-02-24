#include "WGS84.h"

namespace Pave_Libraries_GPSTransforms
{
	// Because double is a non-integral type, definitions must be separated out
	const double WGS84::Flattening = 1 / 298.257223563;
	const double WGS84::SemiMajorAxis = 6378137.0;
	const double WGS84::SemiMinorAxis = WGS84::SemiMajorAxis * (1 - WGS84::Flattening);
	const double WGS84::EccentricitySquared = (2 * WGS84::Flattening) - (WGS84::Flattening * WGS84::Flattening);
	const double WGS84::SecondEccentricitySquared = (1 / (1 - WGS84::EccentricitySquared)) - 1;
}