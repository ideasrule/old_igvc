#include "GPSTransforms.h"

namespace Pave_Libraries_GPSTransforms
{
	const GeodeticPoint& GPSTransforms::localOrigin = GeodeticPoint(ORIGIN_LATITUDE, ORIGIN_LONGITUDE);

	CartesianLocalPoint GPSTransforms::ConvertToLocal(const GeodeticPoint& point)
	{
		return CartesianLocal::GeodeticToLocal(point, localOrigin);
	}
}