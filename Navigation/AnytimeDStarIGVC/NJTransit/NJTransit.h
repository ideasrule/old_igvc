/**************************************************************
 NJTransit.h
 This header file serves as an interface to the path planning 
 module. It contains some initializers and handlers
 Todo: convert the path planner to a generic class and create
 a better interface.

 -Tony
 **************************************************************/

#pragma once
//#define DEBUGMAP

// Handlers

//void StateEstimationHandler(void *data);
//void CostMapHandler(void *data);
//void SetWaypointsHandler(void *data);
void CostMapHandler(void *data);
void MaxSpeedHandler(void *data);
void InitialSearch();
void StateEstimationHandler(void *data);
void SetWaypointsHandler(void *data);
void FindPath(void *data, unsigned long a, unsigned long b);
void ConstructInterpolationTable();

/*****************************************
 variables that needs to be retrieved from configuration file
 *****************************************/
extern double TranslationalSpeed;