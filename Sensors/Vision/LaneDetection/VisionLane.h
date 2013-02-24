
#ifndef VISION_LANE_H
#define VISION_LANE_H

#include "Point2D.h"
#include "gendefs.h"

using namespace Pave_Libraries_Geometry;

enum {VISION_LANE_COEFFS=2};

struct ImageLane
{
	Color color;
	Point2D start, end;
	Point2D pixelStart, pixelEnd;
};

#endif VISION_LANE_H