#include "occupancymaptests.h"

#include <cmath>
#include <wykobi.hpp>

namespace Pave_Libraries_Navigation {

static void assertNonFree(const FovObstacleMap& map)
{
    for(int x = map.minX(); x < map.maxX(); x++) {
        for(int y = map.minY(); y < map.maxY(); y++) {
            GridSquare pt(x, y);
            assert(map.getState(pt) != FovObstacleMap::FREE);
        }
    }
}

void FovObstacleMapTests::inFov()
{
    cout << "testing inFov..." << endl;
    
    FovObstacleMap map(0.1, 100.0, wykobi::PI/3.0);
    
    map.initialize(wykobi::PI/2);
    assert(map.inFieldOfView(GridSquare(2, 0)));
    assert(map.inFieldOfView(GridSquare(10, 1)));
    assert(map.inFieldOfView(GridSquare(40, -20)));
    assert(map.inFieldOfView(GridSquare(88, 44)));

    assert(!map.inFieldOfView(GridSquare(2, 2)));
    assert(!map.inFieldOfView(GridSquare(10, 6)));
    assert(!map.inFieldOfView(GridSquare(40, -24)));
    assert(!map.inFieldOfView(GridSquare(70, 42)));
    assert(!map.inFieldOfView(GridSquare(0, 3)));
    assert(!map.inFieldOfView(GridSquare(0, -3)));
    assert(!map.inFieldOfView(GridSquare(-10, 1)));
    assert(!map.inFieldOfView(GridSquare(-40, -20)));
    assert(!map.inFieldOfView(GridSquare(100, 2)));
    
    cout << "done!" << endl << endl;
}

void FovObstacleMapTests::construction()
{
    cout << "testing construction..." << endl;
    
    FovObstacleMap map(0.1, 400, wykobi::PI/3.0);
    map.initialize(wykobi::PI/2.0);
    
    assertNonFree(map);

    cout << "done!" << endl << endl;
}

} // namespace Pave_Libraries_Navigation
