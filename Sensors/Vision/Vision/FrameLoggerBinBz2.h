#pragma once

#include <string>
#include <memory>

#include "Memory.h"

#include "Frame.h"

#include "FrameLogger.h"

using Pave_Libraries_Common::StateEstimationType;
using Pave_Libraries_Camera::Frame;

class FrameLoggerBinBz2 : public FrameLogger
{
public:
	FrameLoggerBinBz2(const std::string &baseDirectory);
	FrameLoggerBinBz2(const std::string &baseDirectory, const std::string &name);
	virtual ~FrameLoggerBinBz2();
	void log(shared_ptr<Frame> frm, StateEstimationType *state);
	shared_ptr<Frame> readLog(StateEstimationType *state);
private:
	std::fstream manifest;
	int cloudToArray(scoped_array<char> &dest, const cv::Mat_<cv::Point3f>& src);
	void arrayToCloud(cv::Mat_<cv::Point3f>& dest, const scoped_array<char> &src);
};
