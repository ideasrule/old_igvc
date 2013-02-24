
#ifndef FRAME_LOGGER_TEXT_H
#define FRAME_LOGGER_TEXT_H

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

class FrameLoggerText : public FrameLogger
{
public:
	FrameLoggerText(const std::string &baseDirectory);
	FrameLoggerText(const std::string &baseDirectory, const std::string &name);
	virtual ~FrameLoggerText();
	void log(shared_ptr<Frame> frm, StateEstimationType *state);
	shared_ptr<Frame> readLog(StateEstimationType *state);
private:
	std::fstream manifest;
};

#endif //FRAME_LOGGER_TEXT_H