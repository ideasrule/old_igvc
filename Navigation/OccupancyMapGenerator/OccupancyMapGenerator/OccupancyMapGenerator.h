#pragma once

#include "Array2D.h"
#include "OccupancyMap.h"
#include "FovObstacleMap.h"
#include "types.h"

#include "Memory.h"

#include <vector>

namespace Pave_Libraries_Navigation {

class OccupancyMapGenerator
{
public:
    OccupancyMapGenerator(void);
    ~OccupancyMapGenerator(void);

    static CellCount lengthToCells(Length length);
    static Length cellsToLength(CellCount count);

    void updateState(GroundPoint p, Heading h);

    struct CostMapOutput {
        Length x, y;
        Probability pOccupied;

        CostMapOutput(Length x, Length y, Probability p) : x(x), y(y), pOccupied(p) {}
    };
    shared_ptr< std::vector<CostMapOutput> > processStereoPoints(
        std::vector<GroundPoint> points);

    // when the robot approaches the end of the map, replace it with a new one
    // approaching the edge is defined as reaching within
    //   (100 * TRANSLATE_MARGIN * shortest side length of map)% of the edge
    const static double TRANSLATE_MARGIN;
    void translateMap();
    
    const static Probability PROB_OCCUPIED;
    const static Probability PROB_FREE;
    const static Probability PROB_UNKNOWN;
    const static LogOdds LGO_MIN;
    const static LogOdds LGO_MAX;

    static Probability inverseSensorModel(FovObstacleMap::GridSquareState state);

private:
    // stereo updates are processed here before going to the occupancy map
    FovObstacleMap fovMap;

    // maintains the actual occupancy map of the robot's environment
    shared_ptr<OccupancyMap> occupancyMap;

    // the current state of the robot
    bool stateKnown;
    GroundPoint robotPosition;
    Heading robotHeading;

    CellCount xCells();  // # of cells in x direction of occupancy map
    CellCount yCells();  // # of cells in y direction of occupancy map

    GridSquare robotGridSquare();
    GridSquare groundPointToLocalGridSquare(GroundPoint pt);
};

} // namespace Pave_Libraries_Navigation
