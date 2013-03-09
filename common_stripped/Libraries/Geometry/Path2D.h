/*****************************************************************************
 *  Path2D.h
 *  
 *  Provides a data structures and methods to describe a path in 2 dimensions
 *  
 *  This class provides the ability to describe an arbitrary path in 2
 *  dimensions, with no limits on the density of the points other than the
 *  limit of the precision of the double. Functions are provided to retrieve
 *  the closest index to and closest point from another arbitrary point which
 *  need not be on the path. Some functions also can take a distance down the
 *  path. More details can be found in the comments below. There are also
 *  functions to translate and average paths.
 *
 *  NOTE: This class was designed to inherit from the vector class, so it is
 *  possible to push and pop points and use other vector methods in the same
 *  way as with a usual vector.
 *  
 *  Usage:
 *  Path2D path = Path2D(someListOfPoints);
 *  Point2D closestOnPath = path.getClosestPoint(randomPoint);
 *  path.computeTranslation(someX, someY);
 *
 *  Deopendancies: vector
 *
 *  Author: Derrick Yu (ddyu@)
 *****************************************************************************/

#pragma once

#include <vector>

namespace Pave_Libraries_Geometry {
    // forward declarations
    class Point2D;
    struct list_point2d;
    
    // Class comments contained in header
    class Path2D : public std::vector<Point2D*> {
      public:
        Path2D();
        // Create from a list_point2d reference. Can only be used explicitly
        // as a constructor.
        explicit Path2D(list_point2d& path);

        // Create empty path with certain capacity. Can only be used explicitly
        // as a constructor.
        explicit Path2D(int capacity);

        // Create from a vector of points. Can only be used explicitly as a
        // constructor
        explicit Path2D(vector<Point2D*>& points);
    	
        // Return the length of the path. Functions as a semi-accessor. See the
        // note below.
        // NOTE: This function recomputes the path length if the number of
        // points changed after the last request.
        // NOTE: Returns zero if the path has zero points or only one point.
        double getPathLength();
        
        // Returns the average slope of the path from start to end
        // NOTE: This function recomputes the slope if the number of points
        // changed after the last request.
        // NOTE: Returns NULL if the path has zero points or only one point.
        double getAverageSlope();

	    // Computes the length of the path between two indices of the path
        // WARNING: Throws an exception if p1 or p2 are out of bounds of the
        // array of points
        double computeLength(int p1, int p2);

	    // Computes the average curvature of the path (averages the
        // "instantanious" across the path.
	    // WARNING: Throws exception if path has less than three points
	    double computeAverageCurvature();

	    // Computes the approximate signed instantaneous curvature at index 
	    // NOTE: The curvature is signed; positive values indicate a clockwise
	    // bend in the path at this point, and negative values indicate a
	    // counterclockwise bend
	    // WARNING: Throws exception if index is less than one or greater than
        // size() - 1
	    double computeCurvatureAtIndex(int index);
    	
	    // Computes the distance between a Point2D and its closest point on the path
        // WARNING: Throws an exception if computeClosestIndex() returns -1.
	    double computeClosestDistance(const Point2D& point);

	    // Computes the index of the point on the path closest to another
	    // Point2D
        // WARNING: Returns -1 if the pointer is null or the path is empty
	    int computeClosestIndex(const Point2D& point);

	    // Computes the index of the point on the path closest to a distance
	    // down the path
        // WARNING: Returns -1 if the pointer is null or the path is empty
	    int computeClosestIndex(double distance);
    	
	    // Computes the index of the point on the path closest to a distance
	    // down the path from a Point2D. 
        // WARNING: Returns -1 if the pointer is null or the path is empty
        int computeClosestIndex(const Point2D& point, double distance);

	    // Computes the point on the path closest to another Point2D
        // WARNING: Will throw an exception if the path is null or if the
        // calculation of the index returns -1.
	    Point2D* computeClosestPoint(const Point2D& point);

	    // Computes the point on the path closest to a distance down the path
        // WARNING: Will throw an exception if the path is null or if the
        // calculation of the index returns -1.
	    Point2D* computeClosestPoint(double distance);
    	
	    // Computes the point on the path closest to a distance down the path
        // from a Point2D
        // NOTE: Most computationally intensive (see implementation for why)
        // WARNING: Will throw an exception if the path is null or if the
        // calculation of the index returns -1.
	    Point2D* computeClosestPoint(const Point2D& point, double distance);

	    // Computes translation in dx units right and dy units up
        // WARNING: This function mutates this Path2D. If you want a different
        // object that is a translation of this Path2D, use getTraslatedPath().
	    void computeTranslation(double dx, double dy);

	    // Computes translation in dx units right and dy units up
	    // Returns a translated copy of the this Path2D
	    // WARNING: This function returns a completely new Path2D. If you want
        // to mutate this path, use computeTranslation(dx, dy).
	    Path2D* getTranslatedPath(double dx, double dy);
    	
	    // I'm not finished with this, so probably a good idea not to use it.
	    Path2D computeAveragePath(const Path2D& path);
        
        // Takes an open filestream and prints the path to file. The format of
        // the output is:
        // pathLength
        // number of points
        // (x1, y1)
        // ...
        // (xn, yn)
        virtual void printToFile(std::ofstream& stream);

	    // Converts the path into a list_point2d which is the format used to
        // send paths through IPC.
	    list_point2d convert();

    private:
        // Disallow copying
        Path2D(const Path2D&);

        // Disallow assignment
        void operator= (const Path2D&);
        
        // Helper method to copy points from list into path
        void initializeFromList(list_point2d list);

        // The length of the path.
	    double pathLength;
        
        // The average slope of the path
        // NOTE A value of NULL means that there are no points in the path or
        // the path was initialized with no points and the averageSlope has not
        // been accessed.
        double averageSlope;

        // Both variables below keep track of the number of points in the path 
        // the last the respective quantities were calculated. Each quantity
        // gets updated exclusively by their respective accessor methods.
        // if they differ from the value of size().
        // NOTE: A value of -1 means that the capacity of the array has been
        // allocated, but there are no points in the array.
	    size_t pathLengthCount;
        size_t averageSlopeCount;
    };
}  // namespace Pave_Libraries_Geometry