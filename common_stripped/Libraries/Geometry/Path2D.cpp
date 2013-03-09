/*****************************************************************************
 *  Path2D.cpp
 *  
 *  Some of the functions implemnted run as slow as linear time, but most run
 *  below linear time becase they do not need to iterate through the entire
 *  array.
 *
 *  Deopendancies: Path2D.h, fstream, iostream, Path2DException.h, Point2D.h
 *                 PointStructs.h
 *
 *  Author: Derrick Yu (ddyu@)
 *****************************************************************************/

#include "Path2D.h"

#include <fstream>
#include <iostream>

#include "Path2DException.h"
#include "Point2D.h"
#include "PointStructs.h"

namespace Pave_Libraries_Geometry {
    // Constructs an empty Point2D vector, initalizes everything to 0.
    Path2D::Path2D()
	    : vector<Point2D*>()
    {
	    pathLengthCount   = 0;
        averageSlopeCount = 0;
        pathLength        = 0.0;
        averageSlope      = NULL;
    }

    // Constructor copies points from the list into the new path, then sets
    // pointCount to the size and computes initial values of datamembers
    Path2D::Path2D(list_point2d& path)
	    : vector<Point2D*>(path.numPoints, NULL)
    {
	    // copy points from array to path.
        initializeFromList(path);
        pathLengthCount   = 0;
        averageSlopeCount = 0;
        getPathLength();
        getAverageSlope();       
    }

    // Copies the whole entire vector into a new Path2D object. Also
    // initializes private vars to correct values.
    Path2D::Path2D(vector<Point2D*>& points)
	    : vector<Point2D*>(points)
    {
	    averageSlopeCount = 0;
        pathLengthCount   = 0;
	    getPathLength();
        getAverageSlope();
    }

    // Creates an empty path with limited capacity by calling the appropriate
    // vector constructor.
    Path2D::Path2D(int capacity)
	    : vector<Point2D*>(capacity, NULL)
    {
	    averageSlopeCount = 0;
        pathLengthCount   = 0;
        pathLength        = 0.0;
        averageSlope      = NULL;
    }

    // Return length of path. Instead of unnecessarily recalculating the path
    // every time the function is called, it only updates pathLength when the
    // length of the path has changed. Users should understand (and it is
    // written in the header) that changing the number of poinrs in a path
    // and then calling getPathLength() results in a linear time execution,
    double Path2D::getPathLength()
    {
        // return zero if 0 points or one point in path.
        size_t s = size();
        if (s < 1)
            return 0.0;
        
        // update pathLength if points have been added to or removed from path
	    if (pathLengthCount != s) {
		    pathLengthCount = s;
		    pathLength = (int)computeLength(0, (int)pathLengthCount - 1);
	    }
	    return pathLength;
    }

    // the average slope of the path is equivalent to the slope of the line
    // between the endpoints of the path. Checks that the path has more than 1 point
    // for efficiency, the slope only updates when the number of points in the path
    // changes
    double Path2D::getAverageSlope()
    {
        // return zero if 0 points or one point in path.
        size_t s = size();
        if (s < 1)
            return NULL;

        if (averageSlopeCount != s) {
            averageSlopeCount = s;
            double x1 = (*this->begin())->getX();
            double x2 = (*this->end())->getX();
            double y1 = (*this->begin())->getY();
            double y2 = (*this->end())->getY();
            averageSlope =  (y2 - y1) / (x2 - x1);
        }
        return averageSlope;
    }

    // Computes the length of the path between two indices of the path.
    // Performs bounds checking before calculation. Utilizes the Point2D
    // ComputeDistanceTo function and keeps a running total of the distance
    // between adjacent points in the path. Assuming that the points are
    // relatively dense, the approximation is valid.
    // This function uses direct access of the vector instead of calling at(i)
    // because the bounds checking is performed prior to the calculation.
    double Path2D::computeLength(int p1, int p2)
    {
	    // Check to see that p1 and p2 are not negative
	    if (p1 < 0 || p2 < 0)
		    throw IndexNegativeException();

	    // Switch around if p1 is larger
	    if (p1 > p2) {
		    int temp = p1;
		    p1 = p2;
		    p2 = temp;
	    }

	    // Check if p2 is beyond the end of the vector
	    if (p1 > (int)size() - 1 || p2 > (int)size() - 1)
		    throw IndexExceedsLengthException();

    	// calculate distance between adjacent points and add to running total
        // of path length
	    double length = 0.0;
	    for (int i = p1; i < p2; i++)
		    length += (*this)[i]->computeDistanceTo(*(*this)[i + 1]);
	    return length;
    }

    // Computes the average curvature of the path by averaging the
    // "instantanious" curvature for all adjacent triplets in the path.
    // Uses the computeCurvatureAtIndex() function.
    double Path2D::computeAverageCurvature()
    {
	    // Check if path has more than three points
	    if (size() < 3)
		    throw InvalidSize();
    	
	    // calculate sum of instantaneous curvature for all consecutive triplets
        // of Point2D's in the path, then average over the number of times the
        // curvature was calculated
	    double sum = 0.0;
	    for (int i = 1; i < (int)size() - 1; i++)
		    sum += abs(computeCurvatureAtIndex(i));
	    return sum / (size() - 2);  // NOT off by one!
    }

    // The function uses an index whith is the "center" point of the three used
    // to calculate the curvature at that index. Uses Point2D's circucircleRadius
    // method. The sign of the curvture is the same as the sign provied by the
    // circumcircleRadius function
    double Path2D::computeCurvatureAtIndex(int index)
    {
	    // Check if index is negative
	    if (index < 1)
		    throw InvalidInputException();

	    // Check if index is beyond end of array
	    if (index > (int)size() - 1)
		    throw InvalidInputException();
    	
	    // Curvature is 1 / radius of circumscribed circle through points
	    return 1 / Point2D::circumcircleRadius(*this->at(index - 1),
										       *this->at(index),
										       *this->at(index + 1));
    }

    // Calls the corresponding computeClosestIndex function, then computes the
    // distance between the given point and that index using the Point2D
    // distanceTo() funtion. This method of implementation is much more
    // efficent than re-copying all the code from the computeClosestIndex
    // function
    double Path2D::computeClosestDistance(const Point2D& point)
    {
	    int closest = this->computeClosestIndex(point);
        if (closest == -1) {
            throw PointNonexistantException();
        }
	    return (*this)[closest]->computeDistanceTo(point);
    }

    // Computes the index of the point on the path closest to another Point2D
    // According to the golden section search (referenced below). I don't know
    // the actual running time of the algorithm, but it converges faster than
    // the linear method of searching.
    int Path2D::computeClosestIndex(const Point2D& point)
    {
	    // return -1 if path is empty or null
        if (this == NULL || this->size() == 0)
		    return -1;
	    if (&point != 0) {
		    // Golden Section Search, after http://www.nrbook.com/a/bookcpdf/c10-1.pdf

		    const double R = 0.61803399;
		    double C = 1 - R;

		    // Points we're keeping track of
		    int x0, x1, x2, x3;     
		    x0 = 0;
		    x3 = (int)this->size();
		    int bx = (int)(x3 * C);

		    if (x3 - bx > bx - x0) {
			    x1 = bx;
			    x2 = (int)(bx + C * (x3 - bx));
		    }
		    else {
			    x2 = bx;
			    x1 = (int)(bx - C * (bx - x0));
		    }

		    double f1, f2;
		    f1 = point.computeSquaredDistanceTo(*(*this)[x1]);
		    f2 = point.computeSquaredDistanceTo(*(*this)[x2]);

		    // While the interval between the points is larger than 1
		    while (x3 - x0 > 2) {
			    if (f2 < f1) {
				    x0 = x1;
				    x1 = x2;
				    x2 = (int)(R * x1 + C * x3);
				    f1 = f2;
				    f2 = point.computeSquaredDistanceTo(*(*this)[x2]);
			    }
			    else {
				    x3 = x2;
				    x2 = x1;
				    x1 = (int)(R * x2 + C * x0);
				    f2 = f1;
				    f1 = point.computeSquaredDistanceTo(*(*this)[x1]);
			    }
		    }

		    if (f1 < f2)
			    return x1;
		    else
			    return x2;
	    }
	    return -1;
    }

    // The implementation consists of adding consecutive distances to the path
    // until the required distance is reached or surpassed. Then a comparison
    // is made to see which point on the path is actually closer to the distance.
    int Path2D::computeClosestIndex(double distance)
    {
        // initialize index to the end of the array
        size_t index = size() - 1;
	    
        // Check for null pointer or empty path
        if (this == NULL || this->size() == 0)
            return -1;
            
        // Loop through the path, keeping a running sum until the end is
        // reached. Return breaks out of the loop if the desired distance
        // down the path is reached before the end of the path.
        double d = 0.0;
        double diff = 0.0;
	    for (int i = 1; i < (int)size(); i++) {
		    // Calculate the difference between the desired distance and
            // the previous running total
            diff = abs(distance - d);

            // Revise running total.
		    d += (*this)[i]->computeDistanceTo(*(*this)[i - 1]);
            
            // If the new total is greate than the distance, do comparisons
            // to see which point is closer to the actual distance.
		    if (d > distance) {
			    if ((d - distance) > diff) {
				    index = i - 1;
				    return (int)index;
			    }
			    else {
				    index = i;
				    return (int)index;
			    }
		    }
	    }
	    return (int)index;
    }
    	
    // The implementation of this function is different from the previous one.
    // Because any point on the path can be used as the staring point, the
    // function has to take into account the fact that the distance could be
    // in either direction. Surprisingly, The only differences in the loop
    // is the addition of a sign variable and a few modifications.
    // NOTE: Most computationally intensive of all findClosestIndex functions.
    // Calls the computeClosestIndex(point) method then peforms additional
    // calculations
    // TODO: Write a helper function to do closest point finding -Derrick, 7/25/08
    int Path2D::computeClosestIndex(const Point2D& point, double distance)
    {
	    // find the index of the point on the path closest to the given point
	    int closestPointIndex = computeClosestIndex(point);
	    if (distance == 0)
		    return closestPointIndex;

        // Check for null pointer or empty path
	    if (this == NULL || this->size() == 0)
            return -1;

        // set maximum bounds on return value of index depending on direction
	    // of travel along path (sign of distance)
        int index = (distance > 0) ? ((int)size() - 1) : 0;

        // initialize the sign according to whether or not the distance is
        // positive
        int sign = (distance > 0) ? 1 : -1;
        
        // Iterate through the points in the path, keeping a running total
        // of the path length from the starting point. Break out of the
        // loop and return the closest point when distance is reached
        double d = 0.0;                      // Running total of distance
        double diff = 0.0;                   // Previous difference between
                                             // d and absolute distance.
        double absDistance = abs(distance);  // Magnitude of distance
        for (int i = closestPointIndex + sign; i > 0 && i < (int)size() - 1;
             i += sign)
        {
		    diff = absDistance - d;  // Always positive
		    d += (*this)[i]->computeDistanceTo(*(*this)[i - sign]);
            
		    if (d > absDistance) {
			    // compare the difference between distance and the two
			    // closest points
			    if ((d - absDistance) > diff) {
				    index = i - 1;
				    return index;
			    }
			    else {
				    index = i;
				    return index;
			    }
		    }
        }
	    return index;
    }

    // Calls the corresponding computeClosestIndex function, then returns the
    // point at that index. This method of implementation is much more efficent
    // than re-copying all the code from the computeClosestIndex function
    Point2D* Path2D::computeClosestPoint(const Point2D& point)
    {
	    int closest = this->computeClosestIndex(point);
        if (closest == -1) {
            throw PointNonexistantException();
        }
        return (*this)[closest];
    }

    // Calls the corresponding computeClosestIndex function, then returns the
    // point at that index. This method of implementation is much more efficent
    // than re-copying all the code from the computeClosestIndex function
    Point2D* Path2D::computeClosestPoint(double distance)
    {
        int closest = this->computeClosestIndex(distance);
	    if (closest == -1) {
            throw PointNonexistantException();
        }
        return (*this)[closest];
    }

    // Calls the corresponding computeClosestIndex function, then returns the
    // point at that index. This method of implementation is much more efficent
    // than re-copying all the code from the computeClosestIndex function
    // NOTE: This is the most computationally intensive method of computing the
    // closest point because the computeClosestIndex(point, distance) function
    // also calls the computeClosestIndex(point) function.
    Point2D* Path2D::computeClosestPoint(const Point2D& point, double distance)
    {
	    int closest = this->computeClosestIndex(point, distance);
        if (closest == -1) {
            throw PointNonexistantException();
        }
        return (*this)[closest];
    }

    // Iterates through the array and adds dx to each x coordinate, dy to each
    // y coordinate. uses a for loop with direct access instead of iterators
    // because of efficiency reasons.
    void Path2D::computeTranslation(double dx, double dy)
    {
        for (int i = 0; i < (int)size(); i++) {
            (*this)[i]->setX((*this)[i]->getX() + dx);
            (*this)[i]->setY((*this)[i]->getY() + dy);
        }
    }

    // Creates a new path by copying the current path, then calls the
    // computeTranslation() function on the copy. Returns a reference to avoid
    // the use of the copy constructor. The function also copies the points
	// from this path to a new one to avoid using the copy constructor.
    Path2D* Path2D::getTranslatedPath(double dx, double dy)
    {
        Path2D *translatedCopy = new Path2D();
		for (int i = 0; i < (int)size(); i++)
			translatedCopy->push_back((*this)[i]);
        translatedCopy->computeTranslation(dx, dy);
        return translatedCopy;
    }
    	
    // Averages each point in this Path2D with its nearest counterpart in
    // a Path2D path and returns a new Path2D containing the results
    // NOTE: will clip the beginnint and end of both paths such that the
    //		 ends of each path are "even" (details in the implementation)
    /*Path2D Path2D::computeAveragePath(const Path2D& path)
    {
        
        Path2D average = new Path2D();
		    foreach (Point2D p in this)
			    ret.Add((p + path2.ClosestPoint(p)) / 2);
		    return ret;
	    return 0;
    }*/

    // The implementation of the "print" method is done this way because it is
    // just about impossible to insert a for loop into the file stream when
    // overloading the << operator. Therefore, it prints to an open filestream.
    // Sorry, cannot run faster than linear time.
    void Path2D::printToFile(std::ofstream& stream)
    {
	    // Output only if the file exists and is open
	    if (stream.is_open()) {
		    stream << this->getPathLength() << "\n";
		    stream << this->size() << "\n";

		    // Print out the Point2D's in the vector
		    for (int i = 0; i < (int)size(); i++)
			    stream << (*this)[i];
	    }
	    else 
            std::cout << "Unable to open file";
    }

    // To ship things in the IPC network, you have to define the data structure.
    // Therefore, instead of attempting to define a <vector> we just ship the
    // points in our own struct that is a list of point2ds. Of course, this
    // also means that as long as any module ships path data in this format,
    // it can be translated.
    list_point2d Path2D::convert()
    {
	    vector<point2d> points;
	    for (int i = 0; i < (int)this->size(); i++)
		    points[i] = this->at(i)->convert();
    	
	    list_point2d list;
	    list.points = &points[0];
	    list.numPoints = (int)points.size();

	    return list;
    }
    
    // Just copies the points out of the array and adds them to the vector.
    void Path2D::initializeFromList(list_point2d list)
    {
        for (int i = 0; i < list.numPoints; i++)
		    this->push_back(&Point2D(list.points[i]));
    }

}  // namespace Pave_Liraries_Geometry