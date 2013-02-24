#pragma once

#include "Array2D.h"
#include "FloatIndexedArray.h"

#include <vector>
using std::vector;

#include "types.h"

namespace Pave_Libraries_Navigation {

class FovObstacleMapTests;

// Maps all possible fields of view (FOV) around the robot,
//   that is, with the robot in the center and looking at arbitrary directions
// Clients can clear the map, add obstacle points, and then read back free and
//   occupied grid cells. This class will handle marking free points in
//   front of obstacles (between robot and obstacle), and leaving
//   occluded areas untouched (because no new information is found).
// Rather than reconstructing the object for each use, clients should
//   construct once and then follow the steps below.
// Grid cells are assumed to be centered on their (x, y) coordinates.
class FovObstacleMap
{
public:
    enum GridSquareState { OBSTACLE, FREE, UNKNOWN };

    FovObstacleMap(FractionalCellCount minRange, FractionalCellCount maxRange, Angle fov);
    ~FovObstacleMap();

    bool inFieldOfView(GridSquare pt) const;

    // 1. Initialize the map before use
    void initialize(Heading robotHeading);

    // 2. Add obstacle points. Automatically ignores points outside the stereo bound
    void putObstacle(GridSquare pt);

    // 3. Checks whether a cell is free. If a cell is occluded by an obstacle (that is,
    //    there's an obstacle between the robot and the cell), it's NOT considered free.
    //    Returns false for out-of-range points (not in FOV), so callers should be careful
    //    to only pass grid squares in the FOV.
    //    Calls to putObstacle and isKnownFree may be interleaved.
    GridSquareState getState(GridSquare pt) const;

    // minimum and maximum allowed indices
    int minX() const { return map.minX(); }
    int maxX() const { return map.maxX(); }
    int minY() const { return map.minY(); }
    int maxY() const { return map.maxY(); }

private:
    const static int NUMBER_OF_HEADING_BINS = 256;  // TODO: test this, it used to be 3

    struct CellData {
        // the min and max headings that this cell intercepts
        // geometrically, abs(max - min) <= pi if the robot isn't in this cell
        // however, we limit heading to (-pi, pi] so we need to be careful when using these
        Heading minHeading, maxHeading;

        bool isKnownObstacle;
    };

    // updated after each initialize()

    // the map represents the area around the robot
    // the robot is assumed to be in the center of the middle cell of the map
    // map coordinates (say, (3, 2)) refer to the center of the grid square at (3, 2)
    // FractionalGridSquare's are used to refer to the corners, when needed
    Array2D<CellData> map;
    FloatIndexedArray<Length> closestObstacleByHeading;
    Heading robotHeading;    // the robot's current heading

    // constant after construction

    FractionalCellCount maxRange;
    FractionalCellCount minRange;
    Angle fov;

    // helper functions

    // given a grid square, computes the minimum and maximum headings that points in
    //   the grid square intercept
    // this is used in precomputations for finding the closest obstacle in
    //   each direction
    void computeHeadingRangeOfGridPoint(GridSquare pt, Heading& min, Heading& max);

    // in the array of obstacle distances of length N, set all the values to
    //   the minimum of (current value, minLength)
    static void setMinObstacleDistance(Length minLength, Length *distances, int N);

    template<typename PointType>  // for GridSquare and FractionalGridSquare
    Heading computeHeading(PointType pt) const
    {
        Angle angleFromX = atan2((double) pt.y, (double) pt.x);
        if(angleFromX <= -wykobi::PI/2) { angleFromX += 2*wykobi::PI; } // to keep heading in the right range
        return wykobi::PI/2 - angleFromX;
    }

    template<typename PointType>  // for GridSquare and FractionalGridSquare
    FractionalCellCount computeDistance(PointType pt) const
    {
        return wykobi::distance((double) pt.x, (double) pt.y, 0.0, 0.0);
    }

    Heading computeHeadingDiff(Heading h1, Heading h2) const
    {
        Heading headingDiff = abs(h1 - h2);
        if(headingDiff > wykobi::PI) { headingDiff = 2*wykobi::PI - headingDiff; }
        return headingDiff;
    }

    // for testing
    friend FovObstacleMapTests;
};

} // namespace Pave_Libraries_Navigation