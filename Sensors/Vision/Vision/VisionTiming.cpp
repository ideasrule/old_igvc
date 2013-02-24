
#include "VisionTiming.h"

using namespace std;

// parallel with _VisionTimer in VisionTiming.h
static const char *labels[] = {
	"Capture",
	"State",
	"Obs",
	"Color", 
	"Width",
	"Fusion",
	"Fit",
	"Output",
	"Overall"
};

VisionTiming::VisionTiming() {
	for (int i = 0; i < NUM_TIMERS; i++)
		timers[i] = 0;
	iterations = 0;
	name = NULL;
}

void VisionTiming::start(VisionTimer timer) {
	_ftime(&(startTimes[timer]));
}
void VisionTiming::end(VisionTimer timer) {
	_timeb timeStart = startTimes[timer], timeEnd;
	_ftime(&timeEnd);
	int val = timeEnd.time * 1000 + timeEnd.millitm 
		- timeStart.time * 1000 - timeStart.millitm;
	timers[timer] = val;
}
VisionTiming & VisionTiming::operator+=(const VisionTiming &other) {
	for (int i = 0; i < NUM_TIMERS; i++)
		timers[i] += other.timers[i];
    iterations += other.iterations;
	return *this;
}

void VisionTiming::printTo(std::ostream &o, bool withHeaders)
{
	if (iterations == 0 && withHeaders) {
		o << "TIMING INFORMATION (in ms)\n";
		o << "==========================\n";
	}
	if (iterations % 16 == 0 && withHeaders) {
        //o << "Frame#\t";
		for (int i = 0; i < NUM_TIMERS; i++)
			o << labels[i] << '\t';
		o << '\n';
	}
	//o << (iterations++) << '\t';
    iterations++;
	for (int i = 0; i < NUM_TIMERS; i++)
		o << timers[i] << '\t';
	if (name) o << "(" << name << ")";
	o << endl;
}