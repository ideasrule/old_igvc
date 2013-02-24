#pragma once

#include "OccupancyMap.h"
using namespace Pave_Libraries_Navigation;

// fairly lame testing now, should use a test framework
typedef void (*TestFunction)();

// null terminated array of test functions
TestFunction OCCUPANCY_MAP_TESTS[];
