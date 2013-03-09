#pragma once

namespace Pave_Libraries_Geometry
{
    typedef struct point2d
    {
        double x;
        double y;

        point2d(const point2d& p):x(p.x), y(p.y) {}
		point2d(double x, double y):x(x), y(y) {}
		point2d():x(0), y(0) {}
    } point2d;

    typedef struct point3d
    {
        double x;
        double y;
        double z;

        point3d(const point3d& p):x(p.x), y(p.y), z(p.z) {}
		point3d(double x, double y, double z):x(x), y(y), z(z) {}
		point3d():x(0), y(0), z(0) {}
    } point3d;

    typedef struct list_point2d
    {
        int numPoints;
        point2d *points;
    } list_point2d;

    typedef struct list_point3d
    {
        int numPoints;
        point3d *points;
    } list_point3d;
} 