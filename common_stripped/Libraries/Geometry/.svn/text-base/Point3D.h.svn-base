#pragma once

#include <cmath>
#include <iostream>

#include "PointStructs.h"

namespace Pave_Libraries_Geometry {
    class Point2D;
	
    class Point3D {
	    // Friend with ostream class to overload << for C++ I/O
        friend std::ostream &operator << (std::ostream &output, const Point3D &p);

	  public:
		Point3D();
		explicit Point3D(const point3d& p);
        explicit Point3D(const Point2D& p);
		Point3D(double x, double y, double z);
		~Point3D();
		
		// Accessors for coordinates

		double getX();
		void setX(double x);

		double getY();
		void setY(double y);

		double getZ();
		void setZ(double z);

		double computeDistanceTo(Point3D p);
		
		// Convert to a point3d
		point3d convert();

		// Overloaded equality operators
		bool operator == (Point3D);
		bool operator != (Point3D);
		
		// Method is public to give developer option of using it explicitly
		bool equals(Point3D p);

		// Overloaded arithmatic operators
		Point3D operator / (double);
		Point3D operator * (double);
		Point3D operator - ();
		Point3D operator - (Point3D);

		// Support functions for arithmatic operators
		// Public so the developer can have the option of explicity using them
		Point3D scale(double factor);
		Point3D minus(Point3D p);

	private:
		double x;
		double y;
		double z;
	};
} // namespace Pave_Libraries_Geometry