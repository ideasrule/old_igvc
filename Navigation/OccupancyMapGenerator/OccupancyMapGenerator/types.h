#pragma once

#include "wykobi.hpp"

namespace Pave_Libraries_Navigation {

// number of cells on a grid
typedef int CellCount;
// fractional number of cells (for distance measurement)
typedef double FractionalCellCount;

typedef wykobi::point2d<int> GridSquare;
// for example, for finding continuous coordinates for points in a discrete grid
typedef wykobi::point2d<double> FractionalGridSquare;

typedef double Length;
typedef wykobi::point2d<Length> GroundPoint;

typedef double Heading;  // radians
typedef double Angle;    // radians

} // namespace Pave_Libraries_Navigation