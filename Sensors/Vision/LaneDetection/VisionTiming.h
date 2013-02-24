#ifndef TIMING_H
#define TIMING_H

#include <sys/timeb.h>
#include <iostream>

// parallel with labels in VisionTiming.cpp
typedef enum _VisionTimer {
	TIMER_CAPTURE=0,
	TIMER_STATE_CAPTURE, 
	TIMER_OBSTACLE_DETECTION,
	TIMER_LANE_COLOR_FILTER,
	TIMER_LANE_WIDTH_FILTER, 
	TIMER_FUSION, 
	TIMER_PARABOLA_FIT,
	TIMER_OUTPUT,
	TIMER_OVERALL,
	NUM_TIMERS
} VisionTimer;

class VisionTiming
{
public:
	VisionTiming();

	const char *name;
	int timers[NUM_TIMERS];
	_timeb startTimes[NUM_TIMERS];

	void start(VisionTimer timer);
	void end(VisionTimer timer);

	VisionTiming & operator+=(const VisionTiming &other);

	void printTo(std::ostream &o, bool withHeaders = true);

private:
	unsigned int iterations;
};

#endif //TIMING_H