/*****************************************************************************
 *  Point2D.h
 *  
 *  A class containing anylicical geometric methods for 2 dimensional points
 *
 *  Point2D is a two-dimensional point class designed for the manipulation
 *  of cartesian coordinates in two dimensions. The class contains functions
 *  to access/mutate the coordinates, as well as functions to find the
 *  distance and squared distance between points. In lieu of overloading
 *
 *  This class also contains two static functions, which are used primarily
 *  by Path2D to calculate the curvature of the path
 *
 *  Point2D a = Point2D(ax, ay);
 *  Point2D b = Point2D(bx, by);
 *  double distance = &a.computeDistanceTo(&b)
 *
 *  Dependancies: iostream, cmath
 *
 *  Author: Derrick Yu (ddyu@)
 *****************************************************************************/
#pragma once

#include <iostream>
#include <cmath>

namespace Pave_Libraries_Geometry {
    // forward declaration
    struct point2d;

	// Class comments are contained in the header comments
    class Point2D {
        // Friend with ostream class to overload << for logging purposes
        friend std::ostream &operator << (std::ostream &output, const Point2D &p);

      public:
        // Empty constructor initializes both coordinates to 0.0
        Point2D();

        // Constructs a Point2D from a point2d struct.
        explicit Point2D(point2d& p);

        // Constructs a Point2D from cartesian coordinates.
        Point2D(double x, double y);
        
        // access the x coordinate
		double getX() const;

        // mutate the x coordinate
        void setX(double x);
        
        // access the y coordinate
        double getY() const;

        // mutate the y coordinate
        void setY(double y);
        
        // returns the square of the euclidian distance between this point and
        // the given Point2D
        double computeSquaredDistanceTo(const Point2D& p) const;

        // returns the euclidian distance between this point and the given Point2D
        double computeDistanceTo(const Point2D& p) const;
		
		// Convert to a point2d
        point2d convert();
        
        // returns whether the coordinates of this point match the coordinates
        // of the given Point2D, to the precision of 0.0000001 units.
        bool equals(const Point2D& p);

		// returns a new Point2D with coordinates that are this Point2D's
        // coordinates multiplied by a scalar factor.
        Point2D scale(double factor);

        // returns a new Point2D, whose coordinates are the difference between
        // x coordinates and y coordinates of this Point2D and the given Point2D
        Point2D minus(const Point2D& p);

		// The following methods are helpful in determining radius of curvature
		// Mostly used by Path2D

		// Returns true if three points p0-p1-p2 form a counter clockwise turn,
		// (in that order) and false otherwise
		// WARNING: If points are collinear, this also returns false.
		static bool ccw(Point2D& p0, Point2D& p1, Point2D& p2);

		// Returns the radius of the circumcirle passing through points p1, p2,
		// and p3.
		// NOTE: The output is signed: it is positive if p1->p2->p3 forms a 
		//       clockwise turn (eg, the circumcircle lies to the 'right'), and
		//       negative otherwise
        static double circumcircleRadius(Point2D& p1, Point2D& p2, Point2D& p3);

	  private:
        // Disallow copying
        Point2D(const Point2D&);

        // Disallow assignment
        void operator =(const Point2D&);

        // the point's x coordinate, unitless
        double x;

        // the point's y coordinate, unitless
        double y;
	};

    inline double Point2D::getX() const
    {
        return x;
    }

    inline void Point2D::setX(double x)
    {
        this->x = x;
    }

    inline double Point2D::getY() const
    {
        return y;
    }
    
    inline void Point2D::setY(double y)
    {
        this->y = y;
    }

    // Find the the difference between the x coordinates and y coordinates,
    // respectively, between this point and the given point, square them,
    // add to produce the result.
    // Because the difference is squared, there is no need to worry about
    // taking the absolute value of the differences
    // There is a slight tradeoff between beauty and efficiency made here.
    // instead of calling minus to generate what is essentially a difference
    // vector, we instead directly do the subtaction, which is a slight
    // performance bump because there is no need to create new objects and
    // there is one less function call
    inline double Point2D::computeSquaredDistanceTo(const Point2D& p) const
    {
        return (p.x - this->x) * (p.x - this->x) + 
               (p.y - this->y) * (p.y - this->y);
    }

    // The method is the same as above, except for an extra call to take the
    // square root of the whole thing.
    // The function could have been easily implemented by calling the above
    // function and then taking the square root, but that results in a extra
    // function call. This method is a slight bit more efficient.
    inline double Point2D::computeDistanceTo(const Point2D& p) const
    {
        return sqrt((p.x - this->x) * (p.x - this->x) + 
                    (p.y - this->y) * (p.y - this->y));
    }
} // namespace Pave_Libraries_Geometry