#include "FovObstacleMap.h"
#include "OccupancyMapGenerator.h"

#include <cstdlib>

#include "occupancymaptests.h"

using namespace Pave_Libraries_Navigation;

int main(void)
{
    FovObstacleMapTests fovTests;

    fovTests.inFov();
    fovTests.construction();

    system("pause");
}
