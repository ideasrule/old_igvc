#pragma once

#include <wykobi.hpp>

#define X_MAX 200
#define X_MIN (-X_MAX)
#define Y_MAX X_MAX
#define Y_MIN X_MIN

#define UNIT_SIZE .1

#define FOV_DEGREES 60 // degrees
#define FOV_RADIANS (FOV_DEGREES*wykobi::PIDiv180)
#define MINIMUM_RANGE .15  // 2010-11-10: copied from definitions.h, should be measured
#define MAXIMUM_RANGE 10   // 2010-11-10: arbitrary, should be measured --Josh
