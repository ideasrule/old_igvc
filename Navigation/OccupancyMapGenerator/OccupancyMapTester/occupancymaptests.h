#pragma once

#include "FovObstacleMap.h"

#include <cassert>
#include <iostream>

namespace Pave_Libraries_Navigation {

using std::cout;
using std::endl;

class FovObstacleMapTests
{
public:
    void inFov();
    void construction();
    // add test that puts obstacles and checks that free
    //   spaces in front are marked free and occluded spaces
    //   behind are not (and obstacle points themselves are
    //   marked not-free)
    // void addingObstacles();
};

} // namespace Pave_Libraries_Navigation
