#ifndef COST_MAP_GENERATOR_DEFINITIONS_H
#define COST_MAP_GENERATOR_DEFINITIONS_H

#define X_MIN -200
#define X_MAX 200
#define X_UNIT_SIZE .1
#define Y_MIN -200
#define Y_MAX 200
#define Y_UNIT_SIZE .1

#define CLEAR_COST 0.0//5.0//.15 //.141421356 //0 // use sqrt(X_UNIT_SIZE^2 + Y_UNIT_SIZE^2) = maximum distance across the cost cell
#define OBSTACLE_COST .1//200.0 //25.0
#define ADDED_COST 0.0//.1//100.0 //50.0
#define MAX_COST 3.0

#define LANE_ADDED_COST .1
#define BOUNDARY_COST 3.0
#define CLEAR_DISTANCE 10

#define STEREO_X_MIN -15
#define STEREO_X_MAX 15
#define STEREO_X_UNITSIZE .1
#define STEREO_Y_UNITSIZE .1
#define STEREO_Y_MIN 0
#define STEREO_Y_MAX 30//25

#define FOV 60 // degrees
#define MINIMUM_RANGE .15
#define SEGMENTS 40

#define LEARNING_RATE .25

#define WAYPOINT_RADIUS_TO_CLEAR 3 // in cells
#define WAYPOINT_GEN_DISTANCE 1 // in meters, we make a new waypoint when we're this close or closer to the old one

#define LANE_HISTORY_COUNT 3 // number of old lanes to save
#define LANE_WIDTH 10.0 // in meters, from the rules
#define LANE_EXTENT_TO_ESTIMATE 5.0 // in meters, longer if we start on a straightaway, shorter if curve
#define NICENESS_THRESHOLD (-30.0) // minimum niceness, left or right, a lane can have to be valid

#endif