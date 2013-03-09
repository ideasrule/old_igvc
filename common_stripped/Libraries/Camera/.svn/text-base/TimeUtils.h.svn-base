#pragma once

#include <limits.h> 
#include <stdio.h>
#include <sys/timeb.h>

#define SECONDS_PER_DAY (60 * 60 * 24)
#define SUNRISE_TIME_DECIMAL_HOURS 5.6 //5:36 AM
#define SUNSET_TIME_DECIMAL_HOURS 20.4 //8:24 PM



inline double DecimalHours()
{
	_timeb time
	_ftime(&time);		
	long secondsSinceEpoch = time.time;	
	//Go from GMT to EDT by subtracting 4 hours
	secondsSinceEpoch -= 4 * 60 * 60;
	//Get number of seconds since midnight
	long secondsSinceMidnight = secondsSinceEpoch % SECONDS_PER_DAY;
	//to hours
	double hoursSinceMidnight = ((double) secondsSinceMidnight) / (double)3600;
	return hoursSinceMidnight;
}

//Return value is defined as follows: 0 if it is sunrise, 1 if it is sunset, with values in between proportional to amount of time since sunrise. 
inline double DaylightPosition()
{
	double hoursSinceSunrise = DecimalHours() - SUNRISE_TIME_DECIMAL_HOURS;
	return hoursSinceSunrise / (double)(SUNSET_TIME_DECIMAL_HOURS - SUNRISE_TIME_DECIMAL_HOURS);
}