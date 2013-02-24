#pragma once

#define BOUNDS_CHECK 1

#include <cassert>

#include <iostream>

using std::cout;
using std::endl;

#include "OccupancyMap.h"
#include "FloatIndexedArray.h"
using namespace Pave_Libraries_Navigation;

// fairly lame testing now, should use a test framework
typedef void (*TestFunction)();

// null terminated array of test functions
TestFunction OCCUPANCY_MAP_TESTS[];
TestFunction FLOAT_INDEXED_ARRAY_TESTS[];
