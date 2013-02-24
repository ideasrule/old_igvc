#pragma once

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>

#include "Memory.h"

#include "Frame.h"

#include "FrameLogger.h"

using Pave_Libraries_Common::StateEstimationType;
using Pave_Libraries_Camera::Frame;

class FrameLoggerBin32Z : public FrameLogger
{
public:
	FrameLoggerBin32Z(const std::string &baseDirectory);
	FrameLoggerBin32Z(const std::string &baseDirectory, const std::string &name);
	virtual ~FrameLoggerBin32Z();
	void log(shared_ptr<Frame> frm, StateEstimationType *state);
	shared_ptr<Frame> readLog(StateEstimationType *state);
private:
	std::fstream manifest;
};
