/*************************************************************
 Some I/O functions used in debugging the D* algorithm
 Included only when #ifdef DEBUGMAP
 *************************************************************/

#pragma once
#include "DStar.h"
#include "CostMapInt.h"
#include "VehicleDynamics.h"
//TODO: This is the IGVC VehicleDynamics!!!
#include "Geometry.h"


// Prints (to cout) g values of the nodes within the region defined by the min and max. 
void print(DStar::Node ***memory, int minX, int minY, int maxX, int maxY);


// Prints (to file) g values of the nodes within the region defined by the min and max. 
void exportGValues(DStar::Node ***memory, int minX, int minY, int maxX, int maxY );


// Print the cost of the map within the region defined by min and max.
void printCost(Pave_Libraries_Navigation::CostMapInt* costmap, int minX, int minY, int maxX, int maxY);


// Writes the path in tab-separated text file to be plotted in excel or something for easy visualization.
void exportPath(vector<Pave_Libraries_Geometry::point3d>& path); 

// dumps everything (i.e. g, bptr, rhs) around the two nodes
void dump(DStar::Node*** memory, Pave_Libraries_Navigation::CostMapInt *costmap, double x1, double y1, double x2, double y2);


void dumpAll(DStar::Node*** memory, Pave_Libraries_Navigation::CostMapInt *costmap, const char *fileName, double x1, double y1, double x2, double y2);