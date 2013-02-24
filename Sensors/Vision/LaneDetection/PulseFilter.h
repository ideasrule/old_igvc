#pragma once

#include "cv.h"
using cv::Mat_;

#include "Camera.h"
using namespace Pave_Libraries_Camera;

class PulseFilter {
public:
	PulseFilter(int nrows, int ncols);
	~PulseFilter();
	void applyFilter(const Mat_<float> &monoImage, Mat_<float> &laneWidthImage, Camera *cam);
	void applyStoplineFilter(const Mat_<float> &monoImage, const Mat_<float> &laneWidthImage, Mat_<float> &stoplineWidthFilter, Camera *cam);
private:
	int laneWidthForRow(int row, Camera *cam);	
	void stoplineDimensionsForRow(int row, int& width, int& depth, Camera *cam);
	Mat_<double> integralImage;
	double rectangleValue(int lower, int upper, int left, int right);
};
