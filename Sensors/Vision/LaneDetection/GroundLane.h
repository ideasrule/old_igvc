/*****************************************************************************
 *	GroundLane.h
 *
 *	A lane relative to the front of the car on the ground plane.
 *	
 *	This class essentially inherits most of its attributes from the VisionLane
 *	class. The only difference is that the constructor transforms an ImageLane
 *	to the ground plane relative to the car.
 *
 *	Dependancies: Lane.h, Transform.h, Image.h, Geometry.h
 *****************************************************************************/
#pragma once

#include "Vision.h"
#include "Point2D.h"

#include "ImageLane.h"
#include "Transform.h"
#include "Image.h"

using namespace std;
using namespace Pave_Libraries_Geometry;
using namespace Pave_Libraries_Vision;

class GroundLane : public VisionLane
{
public:
	GroundLane(ImageLane * l, Transform * tform);
};

inline GroundLane::GroundLane(ImageLane * l, Transform * tform)
: VisionLane((l->maxRow - l->minRow), l->color, l->confidence)
{
	// For the whole row
	for (int i = 0; i < this->size(); i++)
	{
		Point2D *thisPt = new Point2D();	
		// Then transform to x-y coords
		tform->pixelsToGround(i + l->minRow, l->cols[i + l->minRow], *thisPt);
		this->at(this->size() - i - 1) = thisPt;
	}	
}