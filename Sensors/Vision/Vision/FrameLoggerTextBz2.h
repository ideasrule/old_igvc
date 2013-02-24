#pragma once

#include <string>
#include <memory>

#include "Memory.h"

#include "Frame.h"

#include "FrameLogger.h"

using Pave_Libraries_Common::StateEstimationType;
using Pave_Libraries_Camera::Frame;

class FrameLoggerTextBz2 : public FrameLogger
{
public:
	FrameLoggerTextBz2(const std::string &baseDirectory);
	FrameLoggerTextBz2(const std::string &baseDirectory, const std::string &name);
	virtual ~FrameLoggerTextBz2();
	void log(shared_ptr<Frame> frm, StateEstimationType *state);
	shared_ptr<Frame> readLog(StateEstimationType *state);
private:
	std::fstream manifest;
};
