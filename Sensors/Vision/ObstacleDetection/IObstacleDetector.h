#pragma once

#include "Frame.h"
//#include "PerformanceTimer.h"
#include <vector>

#include "Memory.h"

using std::vector;
using namespace Pave_Libraries_Camera;

class IObstacleDetector
{
public:
	virtual bool process(shared_ptr<Frame> frm) = 0;
    virtual shared_ptr< cv::Mat_<cv::Vec3b> > overlay(void) = 0;
    virtual ~IObstacleDetector() {}
};

