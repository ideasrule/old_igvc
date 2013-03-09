/*****************************************************************************
 *  Point2D.cpp
 *
 *  Most of the implementations are inined in the .h file. The
 *  circumCircleRadius function is particularly intense, since it requires
 *  extensive use of the Newmat library. If depending on Newmat is not desired,
 *  define NO_NEWMAT to replace the function with a dummy implementation.
 *
 *  Dependancies: Point2D.h, newmat.h, PointStructs.h
 *
 *  Author: Derrick Yu (ddyu@)
 *****************************************************************************/
#include "Point2D.h"

#ifndef NO_NEWMAT
#include "newmat.h"
#endif

#include "PointStructs.h"

namespace Pave_Libraries_Geometry {
    Point2D::Point2D()
    {
	    this->x = 0.0;
	    this->y = 0.0;
    }

    Point2D::Point2D(double x, double y)
    {
	    this->x = x;
	    this->y = y;
    }

    // "Convert" from point2d struct
    Point2D::Point2D(point2d& p)
    {
        x = p.x;
	    y = p.y;
    }

    point2d Point2D::convert()
    {
        return point2d(this->x, this->y);
    }

    // As noted below, the function first makes sure that it has not been given
    // a null pointer, then it takes the absolute value of the differences of
    // the coordinates. if both coordinate pairs each differ less than 0.000001
    // (seven digits of precision), then they are considered equal.
    bool Point2D::equals(const Point2D& p)
    {
        // Null pointer case
	    if (&p == 0)
            return false;
	    return (abs(this->x - p.x) < 0.0000001) &&
               (abs(this->y - p.y) < 0.0000001);
    }

    // nothing to see here, some simple multiplication
    Point2D Point2D::scale(double factor)
    {
        return Point2D(this->x * factor, this->y * factor);
    }

    // simple subtraction
    Point2D Point2D::minus(const Point2D& p)
    {
        return Point2D(this->x - p.x, this->y - p.y);
    }

    // The implementation accesses the corrdinates directly instead of using
    // minus() because it results in less overhead with allocating new memory
    // for a new Point2D object.
    bool Point2D::ccw(Point2D& p0, Point2D& p1, Point2D& p2)
    {
        if ((p1.getX() - p0.getX()) * (p2.getY() - p0.getY()) >
            (p1.getY() - p0.getY()) * (p2.getX() - p0.getX()))
            return true;
        return false;
    }

    // The implementation follows the method outlined here: http://en.wikipedia.org/wiki/Circumcircle
    // The difference here is that the c matrix is the b matrix on the reference.
    double Point2D::circumcircleRadius(Point2D& p1, Point2D& p2, Point2D& p3)
#ifdef NO_NEWMAT
	{
		return 0;
	}
#else
    {
        int dim = 3;  // dimension of one side of the matrix. The width and
                      // height are both 3.
        
        // Calculate terms according to cofactor expansion:
        // array of matrix values to stream to matrix
        double aMatrixValues[] = {p1.getX(), p1.getY(), 1,
                                  p2.getX(), p2.getY(), 1,
                                  p3.getX(), p3.getY(), 1};

	    // Matrices need to be initialized to dimensions before streaming in input
        Matrix aMatrix(dim, dim);
        // Stream in values
	    aMatrix << aMatrixValues;
	    double a = aMatrix.Determinant();

        // Treat coordinates like vectors from the origin, calculate the square
        // of the "magnitude" (sum of the squares of the coordinates)
	    double t1 = p1.getX() * p1.getX() + p1.getY() * p1.getY();
	    double t2 = p2.getX() * p2.getX() + p2.getY() * p2.getY();
	    double t3 = p3.getX() * p3.getX() + p3.getY() * p3.getY();

        // Same process as described above to find other determinants
	    double bxMatrixValues[] = {t1, p1.getY(), 1,
                                   t2, p2.getY(), 1,
                                   t3, p3.getY(), 1};
	    Matrix bxMatrix(dim, dim);
	    bxMatrix << bxMatrixValues;
	    double bx = bxMatrix.Determinant();

	    double byMatrixValues[] = {p1.getX(), t1, 1,
                                   p2.getX(), t2, 1,
                                   p3.getX(), t3, 1};
	    Matrix byMatrix(dim, dim);
	    byMatrix << byMatrixValues;
	    double by = byMatrix.Determinant();

	    double cMatrixValues[] = {p1.getX(), p1.getY(), t1,
                                  p2.getX(), p2.getY(), t2,
                                  p3.getX(), p3.getY(), t3};
	    Matrix cMatrix(dim, dim);
	    cMatrix << cMatrixValues;
	    double c = -(cMatrix.Determinant());

	    // NOTE: the center of the circumcircle is at:
	    //		 x0 = bx/(2*a);
	    //		 y0 = by/(2*a);
	    //		 if anyone ever cares...
        
        // Return infinate curvature if three points are about collinear
	    if (abs(a) < 0.00000001)
		    return HUGE_VAL;

        // The ccw() method determines the sign of the curvature.
        // equation is equivalent to the equation described on wikipedia
	    return (ccw(p1, p2, p3) ? -1 : 1) * sqrt((bx * bx) + (by * by) - 4 * a * c) / (2 * abs(a));
    }
#endif //NO_NEWMAT

    // Writing to cout will produce a point in the form "(x, y)"
    // (without the quotation marks, of course)
    std::ostream& operator<<(std::ostream& output, const Point2D& p)
    {
	    output << "(" <<  p.x << ", " << p.y << ")";
	    return output;
    }
} // namespace Pave_Libraries_Geometry