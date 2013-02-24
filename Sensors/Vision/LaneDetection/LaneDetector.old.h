
#ifndef LANE_DETECTOR_H
#define LANE_DETECTOR_H

#include "cv.h"

#include <vector>
#include <sys/timeb.h>

#include "VisionTiming.h"
#include "Frame.h"
#include "ImageLane.h"

using namespace Pave_Libraries_Camera;

class Validator;
class PulseFilter;
class ColorFilter;
class ParabolaFitter;

class LaneDetector
{
public:
	LaneDetector(int nrows, int ncols);
	~LaneDetector();
	void clear();
	bool process(const Frame &frm, VisionTiming &timing);
	std::vector<ImageLane *> lanes, lanesVerified;
	int numWhite;
	int numYellow;
	cv::Mat_<float> yellowImage, whiteImage, roadImage, widthImage;
	cv::Mat_<float> cannyEdgeImage;
	cv::Mat_<float> obstacleImage, fusedYellowImage, fusedWhiteImage;
	//cv::Mat_<float> stoplineWidthFilter;
private:
	int nrows, ncols;
	Validator *validator;
	PulseFilter *laneWidthFilter;
	ColorFilter *laneColorFilter;
	ParabolaFitter *fitter;
};

#endif //LANE_DETECTOR_H