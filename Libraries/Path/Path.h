#pragma once

#include "wykobi.hpp"

#include "Common.h"

//namespace Pave_Libraries_Navigation {

	struct pathPoint
	{
		wykobi::point2d<> p;
		double v;   //speed
		int segment;
	};

	class Path
	{
	private:
		int numSegments;
		wykobi::segment2d *segments;
		vector<double> speed;
		wykobi::ray<wykobi::Float,2> beginningRay;
		wykobi::ray<wykobi::Float,2> endingRay;
	public:
		Path(Pave_Libraries_Geometry::list_point3d p);
		~Path();

		pathPoint Path::closestPoint(const wykobi::point2d<>& p, double *distance);

		//p must be a pathPoint on the path. Returns a point so that the 
		//signed arc length p->m along the path is equal to arclen
		wykobi::point2d<> pointDownPath(pathPoint p, double arclen);

		//Finds the closest point q on the path to the given point p,
		//and then returns the point m so that the signed arc length q->m along the path
		//is equal to arclen
		wykobi::point2d<> pointDownPath(const wykobi::point2d<>& p, double arclen);
		

	};

//}