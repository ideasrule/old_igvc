#include "SimpleOccupancyMap.h"

#include "Memory.h"

namespace Pave_Libraries_Navigation {

const LogOdds SimpleOccupancyMap::INITIAL_VALUE = 0.0;

SimpleOccupancyMap::SimpleOccupancyMap(int xSize, int ySize, LogOdds minValue, LogOdds maxValue) :
	lgoObstacle(xSize, ySize),
    MIN_VALUE(minValue),
    MAX_VALUE(maxValue)
{
	clear();
}

SimpleOccupancyMap::SimpleOccupancyMap(int xMin, int xMax, int yMin, int yMax,
                           LogOdds minValue, LogOdds maxValue) :
	lgoObstacle(xMin, xMax, yMin, yMax),
    MIN_VALUE(minValue),
    MAX_VALUE(maxValue)
{
	clear();
}

SimpleOccupancyMap::~SimpleOccupancyMap() {}

void SimpleOccupancyMap::clear()
{
    lgoObstacle.setAll(INITIAL_VALUE);
}

void SimpleOccupancyMap::update(int x, int y, Probability inverseSensorModelValue)
{
    // for convenience
    LogOdds& lgo = lgoObstacle.cell(x, y);
    lgo += probabilityToLogOdds(inverseSensorModelValue);
    if(lgo > MAX_VALUE) { lgo = MAX_VALUE; }
    if(lgo < MIN_VALUE) { lgo = MIN_VALUE; }
}

LogOdds SimpleOccupancyMap::lgoOccupied(int x, int y) const
{
    return lgoObstacle.cell(x, y);
}

Probability SimpleOccupancyMap::pOccupied(int x, int y) const
{
    return logOddsToProbability(lgoObstacle.cell(x, y));
}

} // namespace Pave_Libraries_Navigation
