#pragma once

#include "Memory.h"

#include "Frame.h"
using Pave_Libraries_Camera::Frame;

#include <cv.h>

class ImageViewer
{
public:
	ImageViewer(void);
	~ImageViewer(void);

	void giveImage(const cv::Mat_<cv::Vec3b>&);
};
