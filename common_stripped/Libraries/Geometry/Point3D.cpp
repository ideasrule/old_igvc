#include "Point3D.h"

#include "Point2D.h"

namespace Pave_Libraries_Geometry
{
    // "Default" Constructor
    Point3D::Point3D()
    {
        this->x = 0.0;
        this->y = 0.0;
        this->z = 0.0;
    }

    // Construct with coordinates
    Point3D::Point3D(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    // Convert from point3d
    Point3D::Point3D(const point3d& p)
    {
        x = p.x;
        y = p.y;
        z = p.z;
    }

    // Give some more dimension to the traditional Point2D
    Point3D::Point3D(const Point2D& p)
    {
        x = p.getX();
        y = p.getY();
        z = 0.0;
    }

    // Destructor
    Point3D::~Point3D()
    {
        delete &x, &y, &z;
    }

    // Accessor methods for coordinates

    inline double Point3D::getX()
    {
        return x;
    }

    inline void Point3D::setX(double x)
    {
        this->x = x;
    }

    inline double Point3D::getY()
    {
        return y;
    }

    inline void Point3D::setY(double y)
    {
        this->y = y;
    }

    inline double Point3D::getZ()
    {
        return z;
    }

    inline void Point3D::setZ(double z)
    {
        this->z = z;
    }

    // Euclidean distance between two Point3D's
    double Point3D::computeDistanceTo(Point3D p)
    {
        Point3D vector = this->minus(p);
        return sqrt(((vector.x) * (vector.x)) + ((vector.y) * (vector.y)) +
                    ((vector.z) * (vector.z)));
    }

    // Convert to a point3d
    point3d Point3D::convert()
    {
        point3d p;
        p.x = this->x;
        p.y = this->y;
        p.z = this->z;

        return p;
    }

    // Overloaded Equality Operators

    // is equal to returns false when one point is null
    bool Point3D::operator == (Point3D p)
    {
        // return true if both null
        if (this == 0 && &p == 0)
    	    return true;
        // return false if only one is null
        else if (this == 0 || &p == 0)
            return false;
        return this->equals(p);
    }

    // is not equal to returns true when one point is null
    bool Point3D::operator != (Point3D p)
    {
        // return false if both null
        if (this == 0 && &p == 0)
            return false;
        // return true if only one is null
        else if (this == 0 || &p == 0)
            return true;
        return !this->equals(p);
    }

    // Overloaded Arithmatic Operators
    bool Point3D::equals(Point3D p)
    {
        // Null pointer case
        if (&p == 0)
            return false;
        return (abs(this->x - p.x) < .0000001) && 
               (abs(this->y - p.y) < .0000001) && 
               (abs(this->z - p.z) < .0000001);
    }

    // Divide by a factor
    Point3D Point3D::operator / (double factor)
    {
        return this->scale(1 / factor);
    }

    // Multiply by a factor
    Point3D Point3D::operator * (double factor)
    {
        return this->scale(factor);
    }

    // Reflect over origin
    Point3D Point3D::operator -()
    {
        return this->scale(-1.0);
    }

    // "minus" operator creates a difference vector
    Point3D Point3D::operator - (Point3D p)
    {
        return this->minus(p);
    }

    // Helper functions for operator overloads

    // Scale a Point3D by a factor
    Point3D Point3D::scale(double factor)
    {
        return * new Point3D(this->x * factor, this->y * factor,
                             this->z * factor);
    }

    // Create a difference vector from two Point3D's
    Point3D Point3D::minus(Point3D p)
    {
	    return * new Point3D(this->x - p.x, this->y - p.y, this->z - p.z);
    }

    // Overloaded << operator for C++ I/O
    std::ostream& operator<<(std::ostream& output, const Point3D& p)
    {
        output << "(" <<  p.x << ", " << p.y << ", " << p.z <<")";
        return output;
    }
} // namespace Pave_Libraries_Geometry