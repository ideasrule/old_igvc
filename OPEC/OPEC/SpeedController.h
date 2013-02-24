#pragma once

#include "MedianFilter.h"
#include "PID.h"

using namespace Pave_OPEC_PID;

#define LOGGING_ROOT "C:\\IGVC\\logs\\"

class SpeedController
{
public:
	typedef double (*FeedForwardFunction)(double);

	SpeedController(bool left);
	~SpeedController(void);

	// for disabled/paused
	void reset(void);
	void setDesired(double desired);

	double getDesired(void);
	double getOutput(double wheelSpeed);

	static double feedForwardLeft(double desSpeed);
	static double feedForwardRight(double desSpeed);

private:
	PID *pid;
	FeedForwardFunction ff;
};
