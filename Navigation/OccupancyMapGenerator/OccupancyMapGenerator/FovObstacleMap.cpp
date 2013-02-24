#include "FovObstacleMap.h"

#include <cmath>

#include <iostream>  // for debugging only

namespace Pave_Libraries_Navigation {

template<typename T>
int intCeil(T t) { return static_cast<int>(std::floor(t)); }

FovObstacleMap::FovObstacleMap(FractionalCellCount minRange, FractionalCellCount maxRange, Angle fov) :
    map(-intCeil(maxRange), intCeil(maxRange), -intCeil(maxRange), intCeil(maxRange)),
    fov(fov), maxRange(maxRange), minRange(minRange),
    closestObstacleByHeading(NUMBER_OF_HEADING_BINS, -wykobi::PI, wykobi::PI)
{
    for(int x = map.minX(); x < map.maxX(); x++) {
        for(int y = map.minY(); y < map.maxY(); y++) {
            GridSquare pt(x, y);
            CellData& cellData = map.cell(x, y);
            computeHeadingRangeOfGridPoint(pt, cellData.minHeading, cellData.maxHeading);
        }
    }
}



FovObstacleMap::~FovObstacleMap(void)
{
    // empty
}



bool FovObstacleMap::inFieldOfView(GridSquare pt) const
{
    FractionalCellCount distance = computeDistance(pt);
    return distance < maxRange && distance > minRange
        && computeHeadingDiff(computeHeading(pt), robotHeading) < fov/2;
}



// initializes the map (clearing any previous obstacles)
// the heading is required because the specific grid cells that obstacles
//   occupy can vary
void FovObstacleMap::initialize(Heading robotHeading)
{
    this->robotHeading = robotHeading;

    // set all points to unknown
    for(int x = minX(); x < maxX(); x++) {
        for(int y = minY(); y < maxY(); y++) {
            CellData& cellData = map.cell(x, y);
            cellData.isKnownObstacle = false;
        }
    }
}



// add a obstacle square to the map, marking its distance in the relevant heading bins
void FovObstacleMap::putObstacle(GridSquare pt)
{
    if(!inFieldOfView(pt)) {
//std::cerr << "out of FOV: d = " << computeDistance(pt) << ", range = (" << minRange << ", " << maxRange << "), hdiff = " << computeHeadingDiff(computeHeading(pt), robotHeading) << ", fov/2 = " << fov/2.0 << std::endl;
        return;  // we don't want to consider this point
    }
    
    CellData& cellData = map.cell(pt.x, pt.y);
    cellData.isKnownObstacle = true;
    Length distance = computeDistance(pt);

    // if the difference between max and min is greater than pi, the cell lies 
    //  across the discontinuity in heading so array access must be separate
    Length *minObstacleDistances;
    int N;
    if(cellData.maxHeading - cellData.minHeading > wykobi::PI)
    {
        N = closestObstacleByHeading.cells(-wykobi::PI, cellData.minHeading, minObstacleDistances);
        setMinObstacleDistance(distance, minObstacleDistances, N);

        N = closestObstacleByHeading.cells(cellData.minHeading, wykobi::PI, minObstacleDistances);
        setMinObstacleDistance(distance, minObstacleDistances, N);
    }
    else
    {
        N = closestObstacleByHeading.cells(cellData.minHeading, cellData.maxHeading, minObstacleDistances);
        setMinObstacleDistance(distance, minObstacleDistances, N);
    }
}



FovObstacleMap::GridSquareState FovObstacleMap::getState(GridSquare pt) const
{
    if(!inFieldOfView(pt)) { return UNKNOWN; }
    
    const CellData& cellData = map.cell(pt.x, pt.y);

    if(cellData.isKnownObstacle) {
        return OBSTACLE;
    }
    // if this is not an obstacle, it's considered free if there are no obstacles
    //   between this and the robot (it's not occluded)
    else if(computeDistance(pt) < closestObstacleByHeading.cell(computeHeading(pt))) {
        return FREE;
    }
    else {
        return UNKNOWN;
    }
}



// assumes cell headings are already computed
void FovObstacleMap::computeHeadingRangeOfGridPoint(
    GridSquare pt, Heading& minHeading, Heading& maxHeading)
{
    // the coordinates of the four corners of this grid point
    FractionalGridSquare corners[] = {
        FractionalGridSquare(pt.x - 0.5, pt.y + 0.5),  // top left
        FractionalGridSquare(pt.x + 0.5, pt.y + 0.5),  // top right
        FractionalGridSquare(pt.x - 0.5, pt.y - 0.5),  // bottom left
        FractionalGridSquare(pt.x + 0.5, pt.y - 0.5)   // bottom right
    };

    // all of the possible pairs of corners
    // we're interested in the one with the largest heading difference between corners
    // we can't just take min and max heading of the corners, because of the
    //   discontinuity in headings at +/-pi
    int cornerPairs[6][2] = {  // the length 6 is used below
        {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}
    };

    int maxIndex = -1;
    Heading maxHeadingDiff = -1; // valid values are never be negative
    for(int i = 0; i < 6; i++) {
        FractionalGridSquare c1 = corners[cornerPairs[i][0]];
        FractionalGridSquare c2 = corners[cornerPairs[i][1]];
        if(computeHeadingDiff(computeHeading(c1), computeHeading(c2)) > maxHeadingDiff) {
            maxIndex = i;
        }
    }

    Heading h1 = computeHeading(corners[cornerPairs[maxIndex][0]]);
    Heading h2 = computeHeading(corners[cornerPairs[maxIndex][1]]);
    minHeading = std::min(h1, h2);
    maxHeading = std::max(h1, h2);
}



void FovObstacleMap::setMinObstacleDistance(Length minLength, Length *distances, int N)
{
    for(int i = 0; i < N; i++) {
        distances[i] = std::min(minLength, distances[i]);
    }
}



} // namespace Pave_Libraries_Navigation