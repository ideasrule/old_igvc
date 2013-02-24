#include "OccupancyMapGenerator.h"

#include "wykobi.hpp"
#include <cmath>
#include <iostream>

#include "MappingConstants.h"
#include "SimpleOccupancyMap.h"
#include "TiledOccupancyMap.h"

namespace Pave_Libraries_Navigation {

#define TILE_SIZE (1<<12)

OccupancyMapGenerator::OccupancyMapGenerator(void) :
    fovMap(lengthToCells(MINIMUM_RANGE), lengthToCells(MAXIMUM_RANGE), FOV_RADIANS),
    //occupancyMap(new SimpleOccupancyMap(lengthToCells(X_MIN), lengthToCells(X_MAX),
    //             lengthToCells(Y_MIN), lengthToCells(Y_MAX), LGO_MIN, LGO_MAX)),
    occupancyMap(new TiledOccupancyMap(lengthToCells(X_MIN), lengthToCells(X_MAX),
                 lengthToCells(Y_MIN), lengthToCells(Y_MAX), TILE_SIZE, LGO_MIN, LGO_MAX)),
    stateKnown(false)
{
}

OccupancyMapGenerator::~OccupancyMapGenerator(void) {}

CellCount OccupancyMapGenerator::lengthToCells(Length length)
{
    return (CellCount) ceil(length / UNIT_SIZE);
}

Length OccupancyMapGenerator::cellsToLength(CellCount count)
{
    return ((Length) count + 0.5) * UNIT_SIZE;
}

void OccupancyMapGenerator::updateState(GroundPoint p, Heading h)
{
    robotPosition = p;
    robotHeading = h;
    stateKnown = true;

    // update the map if the robot has reached the end/gone outside
    // must be called after robotPosition is updated
    translateMap();
}

typedef std::vector<OccupancyMapGenerator::CostMapOutput> CostMapOutputVector;
shared_ptr<CostMapOutputVector>
OccupancyMapGenerator::processStereoPoints(std::vector<GroundPoint> points)
{
    if(!stateKnown) {
        std::cerr << "Received stereo points but state not known, ignoring" << std::endl;
        return shared_ptr<CostMapOutputVector>();
    }

    fovMap.initialize(robotHeading);

    typedef std::vector<GroundPoint>::const_iterator It;
    for(It it = points.begin(); it != points.end(); it++) {
        fovMap.putObstacle(groundPointToLocalGridSquare(*it));
    }

    // iterate over all squares
    // check if they're obstacles
    // write to the appropriate new values to the map, checking thresholds
    // make a list of things to return for IPC transmission

    shared_ptr< std::vector<CostMapOutput> > updates(new std::vector<CostMapOutput>());

    for(int x = fovMap.minX(); x < fovMap.maxX(); x++) {
        for(int y = fovMap.minY(); y < fovMap.maxY(); y++) {
            GridSquare gs(x, y);
            if(!fovMap.inFieldOfView(gs)) { continue; }

            FovObstacleMap::GridSquareState state = fovMap.getState(gs);
            if(state != FovObstacleMap::UNKNOWN) {
                GridSquare global = robotGridSquare() + gs;
                int gx = global.x, gy = global.y;
                occupancyMap->update(gx, gy, inverseSensorModel(state));

                // save the update
                updates->push_back(CostMapOutput(
                    cellsToLength(gx), cellsToLength(gy), occupancyMap->pOccupied(gx, gy)));
            }
        }
    }

    return updates;
}

const double OccupancyMapGenerator::TRANSLATE_MARGIN = 0.05;

void OccupancyMapGenerator::translateMap()
{
    double shortestSide = (double) std::min(
        occupancyMap->maxX() - occupancyMap->minX(),
        occupancyMap->maxY() - occupancyMap->minY());
    int minAllowedMargin = lengthToCells(MAXIMUM_RANGE) + 
        static_cast<int>(shortestSide * TRANSLATE_MARGIN);

    GridSquare robotSquare = robotGridSquare();
    int rx = robotSquare.x;
    int ry = robotSquare.y;

    int sideMargins[] = {
        occupancyMap->maxX() - rx,
        rx - occupancyMap->minX(),
        occupancyMap->maxY() - ry,
        ry - occupancyMap->minY()
    };
    int *minMargin = std::min_element(sideMargins, sideMargins + 4);

    if(*minMargin < minAllowedMargin) {
        //occupancyMap = shared_ptr<OccupancyMap>(new SimpleOccupancyMap(
        //    rx + lengthToCells(X_MIN), rx + lengthToCells(X_MAX),
        //    ry + lengthToCells(Y_MIN), ry + lengthToCells(Y_MAX),
        //    LGO_MIN, LGO_MAX));
        occupancyMap = shared_ptr<OccupancyMap>(new TiledOccupancyMap(
            rx + lengthToCells(X_MIN), rx + lengthToCells(X_MAX),
            ry + lengthToCells(Y_MIN), ry + lengthToCells(Y_MAX),
            TILE_SIZE, LGO_MIN, LGO_MAX));
    }
}

GridSquare OccupancyMapGenerator::robotGridSquare()
{
    return GridSquare(lengthToCells(robotPosition.x), lengthToCells(robotPosition.y));
}

GridSquare OccupancyMapGenerator::groundPointToLocalGridSquare(GroundPoint pt)
{
    double headingInDegrees = robotHeading / wykobi::PIDiv180;
    GroundPoint globalDiffPt = wykobi::rotate(-headingInDegrees, pt);

    return GridSquare(lengthToCells(globalDiffPt.x), lengthToCells(globalDiffPt.y));
}

// arbitrarily chosen as of 2010-11-21, --Josh
const Probability OccupancyMapGenerator::PROB_OCCUPIED = 0.7;
const Probability OccupancyMapGenerator::PROB_FREE = -0.7;
const Probability OccupancyMapGenerator::PROB_UNKNOWN = 0.0;
const LogOdds OccupancyMapGenerator::LGO_MIN = -5.0;
const LogOdds OccupancyMapGenerator::LGO_MAX = 5.0;

Probability OccupancyMapGenerator::inverseSensorModel(FovObstacleMap::GridSquareState state)
{
    switch(state) {
        case FovObstacleMap::OBSTACLE: return PROB_OCCUPIED;
        case FovObstacleMap::FREE: return PROB_FREE;
        case FovObstacleMap::UNKNOWN:  // fall through
        default: return PROB_UNKNOWN;
    }
}

} // namespace Pave_Libraries_Navigation