#include "Path.h"

#include <cstdio>
#include <iostream>
#include <limits>

using namespace wykobi;
using namespace Pave_Libraries_Geometry;

//namespace Pave_Libraries_Navigation {

	Path::Path(list_point3d inputPath)
	{
		numSegments = inputPath.numPoints-1; 
		segments = new segment2d[numSegments];
		speed.resize(inputPath.numPoints);

		for(int i = 0; i < numSegments; i++)
		{
			wykobi::point2d<> p1(inputPath.points[i].x, inputPath.points[i].y);
			wykobi::point2d<> p2(inputPath.points[i+1].x, inputPath.points[i+1].y);
			segments[i] = make_segment(p1,p2);
			speed[i] = inputPath.points[i].z;
		}
		speed[numSegments] = inputPath.points[numSegments].z;
		
		//Make rays for beginning and end

		wykobi::point2d<> ptemp = segments[0][0]-segments[0][1];
		wykobi::vector2d<Float> v = make_vector(ptemp);
		beginningRay = make_ray<Float>(segments[0][1],v);

		ptemp = segments[numSegments-1][1]-segments[numSegments-1][0];
		v = make_vector(segments[numSegments-1][1]-segments[numSegments-1][0]);
		endingRay = make_ray<Float>(segments[numSegments-1][0],v);
	}

	Path::~Path()
	{
		delete[] segments;
	}

	//Returns the closest point on the path, the index of the segment, and the unsigned distance (by ref)
	pathPoint Path::closestPoint(const wykobi::point2d<>& p, double *distance)
	{
		//Find nearest point on the path
		float minDist = std::numeric_limits<float>::infinity();
		pathPoint closestPathPoint;

		for(int i = 0; i < numSegments; i++)
		{
			wykobi::point2d<> challenger;

			if(i == 0) //replace beginning with ray
				challenger = closest_point_on_ray_from_point(beginningRay, p);
			else if(i == numSegments-1) //replace end with ray
				challenger = closest_point_on_ray_from_point(endingRay, p); //closest_point_on_ray_from_point(segments[numSegments-1][0].x, segments[numSegments-1][0].y, segments[numSegments-1][1].x-segments[numSegments-1][0].x, segments[numSegments-1][1].y-segments[numSegments-1][0].y, p.x, p.y);
			else
				challenger = closest_point_on_segment_from_point(segments[i], p);

			float dist = wykobi::distance(challenger, p); //Could speed up by comparing square of distances
			if(dist < minDist)
			{
				closestPathPoint.segment = i;
				closestPathPoint.p = challenger;
				closestPathPoint.v = speed[i];
				minDist = dist;
			}
		}
		
		*distance = minDist;
		return closestPathPoint;
	}


	//Finds the closest point q on the path to the given point p,
	//and then returns the point m so that the signed arc length q->m along the path
	//is equal to arclen
	wykobi::point2d<> Path::pointDownPath(pathPoint p, double arclen)
	{
		//Travel the appropriate arc length

		wykobi::point2d<> currPoint = p.p;
		int seg = p.segment;
		int dir = arclen > 0 ? 1 : -1;
		arclen = abs(arclen);

		for(; seg >= 0 && seg < numSegments; seg += dir)
		{
			float segmentLength = wykobi::distance(currPoint, segments[seg][dir==1?1:0]);
			if((seg == 0) && (dir==-1)) // Handle ray at beginning
				return project_point(currPoint, currPoint + segments[seg][0] - segments[seg][1], arclen);
			else if((seg == numSegments - 1) && (dir==1)) //Handle ray at end
				return project_point(currPoint, currPoint + segments[seg][1] - segments[seg][0], arclen);
			if(arclen < segmentLength) //Point is within this segment, accounting for rays at beginning and end
				return project_point(currPoint, segments[seg][dir==1?1:0], arclen);
			
			currPoint = segments[seg][dir==1?1:0];
			arclen -= segmentLength;
		}
		
	}

		


	//Finds the closest point q on the path to the given point p,
	//and then returns the point m so that the signed arc length q->m along the path
	//is equal to arclen
	wykobi::point2d<> Path::pointDownPath(const wykobi::point2d<>& p, double arclen)
	{
		//Find nearest point on the path
		double distToPoint;
		pathPoint cp = closestPoint(p, &distToPoint);

		//Travel the appropriate arc length
		return pointDownPath(cp, arclen);
		
	}

//}

	

	
