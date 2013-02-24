#include "TiledOccupancyMap.h"

#include <cmath>

namespace Pave_Libraries_Navigation {

static int halfNumTiles(int min, int max, int tileSize)
{
    return -1 + (int) ceil((max - min) / (double) tileSize / 2.0);
}

TiledOccupancyMap::TiledOccupancyMap(int xMin, int xMax, int yMin, int yMax,
    int tileSize, LogOdds minValue, LogOdds maxValue) :
    oddTileSize((tileSize & ~1) + 1),
    simpleMaps(-halfNumTiles(xMin, xMax, tileSize), halfNumTiles(xMin, xMax, tileSize),
        -halfNumTiles(yMin, yMax, tileSize), halfNumTiles(yMin, yMax, tileSize))
{
    int x0 = (xMin + xMax) / 2;
    int y0 = (yMin + yMax) / 2;
    int halfTileSize = oddTileSize / 2;

    for(int x = simpleMaps.minX(); x < simpleMaps.maxX(); x++) {
        for(int y = simpleMaps.minY(); y < simpleMaps.maxY(); y++) {
            int tx = x0 + oddTileSize*x;
            int ty = y0 + oddTileSize*y;
            simpleMaps.cell(x, y) = new SimpleOccupancyMap(
                tx - halfTileSize, tx + halfTileSize,
                ty - halfTileSize, ty + halfTileSize,
                minValue, maxValue);
        }
    }
}

void TiledOccupancyMap::update(int x, int y, Probability inverseSensorModelValue)
{
    int tx = x / oddTileSize;
    int ty = y / oddTileSize;
    simpleMaps.cell(tx, ty)->update(x, y, inverseSensorModelValue);
}

Probability TiledOccupancyMap::pOccupied(int x, int y) const
{
    int tx = x / oddTileSize;
    int ty = y / oddTileSize;
    return simpleMaps.cell(tx, ty)->pOccupied(x, y);
}

Probability TiledOccupancyMap::lgoOccupied(int x, int y) const
{
    int tx = x / oddTileSize;
    int ty = y / oddTileSize;
    return simpleMaps.cell(tx, ty)->lgoOccupied(x, y);
}

TiledOccupancyMap::~TiledOccupancyMap(void)
{
    for(int x = simpleMaps.minX(); x < simpleMaps.maxX(); x++) {
        for(int y = simpleMaps.minY(); y < simpleMaps.maxY(); y++) {
            delete simpleMaps.cell(x, y);
        }
    }
}

} // namespace Pave_Libraries_Navigation
