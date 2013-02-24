
#ifndef FRAME_LOGGER_H
#define FRAME_LOGGER_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <memory>
#include <time.h>

#include "Memory.h"

#include "Frame.h"

namespace Pave_Libraries_Common {
    struct StateEstimationType;
}
using Pave_Libraries_Common::StateEstimationType;
using Pave_Libraries_Camera::Frame;
using std::string;

static bool printDebug = true;

class FrameLogger
{
public:
	FrameLogger(const std::string &baseDirectory)
		: name(nameForCurrentTime()), directory(baseDirectory + "/" + name + "-VISION") {}
	FrameLogger(const std::string &baseDirectory, const std::string &name)
		: name(name), directory(baseDirectory + "/" + name + "-VISION") {}
	virtual ~FrameLogger() {}
	virtual void log(shared_ptr<Frame> frm, StateEstimationType *state) = 0;
	virtual shared_ptr<Frame> readLog(StateEstimationType *state) = 0;
	const std::string getName() { return name; }
	const std::string getDirectory() { return directory; }

private:
	std::string name;
	const std::string directory;

	static std::string nameForCurrentTime()
	{
		time_t theTime;
		time(&theTime);
		tm localTime;
		localtime_s(&localTime, &theTime);
		char filename[1000];
		sprintf_s(filename, 1000, "%04d-%02d-%02d-%02d-%02d-%02d", localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, localTime.tm_hour, localTime.tm_min, localTime.tm_sec);
		return string(filename);
	}
};

#endif //FRAME_LOGGER_H