#pragma once

#include "OccupancyMap.h"
#include "SimpleOccupancyMap.h"
#include "Array2D.h"

namespace Pave_Libraries_Navigation {

class TiledOccupancyMap : public OccupancyMap
{
public:
    TiledOccupancyMap(int xMin, int xMax, int yMin, int yMax,
        int tileSize, LogOdds minValue, LogOdds maxValue);
    virtual ~TiledOccupancyMap(void);

    virtual void update(int x, int y, Probability inverseSensorModelValue);
    virtual Probability pOccupied(int x, int y) const;
    virtual LogOdds lgoOccupied(int x, int y) const;

    virtual int minX() const { return (int) (simpleMaps.minX() - 0.5)*oddTileSize; }
    virtual int maxX() const { return (int) (simpleMaps.maxX() + 0.5)*oddTileSize; }
    virtual int minY() const { return (int) (simpleMaps.minY() - 0.5)*oddTileSize; }
    virtual int maxY() const { return (int) (simpleMaps.maxY() + 0.5)*oddTileSize; }

private:
    Array2D<SimpleOccupancyMap *> simpleMaps;
    int oddTileSize;
};

} // namespace Pave_Libraries_Navigation
