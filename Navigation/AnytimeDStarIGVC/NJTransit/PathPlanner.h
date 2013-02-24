#pragma once

#include "NJTransit.h"
#include "Common.h"
#include "DStar.h"
#include "DStarPriorityQueue.h"
#include "Path.h"
#include "Lattice.h"

#include "CostMapInt.h"
#include "VehicleDynamics.h"
//TODO: This is the IGVC VehicleDynamics!!!

#include "FloatingPointMath.h"
#include "wykobi.hpp"
#include <vector>
#include <list>
#include <algorithm>

//#ifdef DEBUGMAP
	#include "debug.h"
//#endif

using namespace DStar;
using namespace Pave_Libraries_Navigation;
using std::vector;

/*****************************************
 Constant definitions
 *****************************************/
#define LARGE_INT 2000000000    //anything above this is infinity
#define HARD_OBST_THRESHOLD 10000 //this (or above) indicates a hard obstacle

#define STRAIGHT 100.0
#define DIAG 141.4213562
#define HALF_DIAG 111.8033989

#define MAX_COST 280.0
#define MAX_TIMES_EXPANDED 2000000