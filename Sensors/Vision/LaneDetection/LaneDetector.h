
#ifndef LANE_DETECTOR_H
#define LANE_DETECTOR_H

#include "cv.h"
#include "Memory.h"

#include <vector>
#include <sys/timeb.h>

#include "VisionTiming.h"
#include "Frame.h"
#include "ImageLane.h"
#include "ColorFilter.h"
#include "GroundPolyFitter.h"
#include "Validator.h"
#include "Spline.h"

using namespace Pave_Libraries_Camera;

class LaneDetector
{
public:
	LaneDetector(int nrows, int ncols);
	~LaneDetector();
	void clear();
	bool process(shared_ptr<Frame> frm, VisionTiming &timing);
//	std::vector<ImageLane *> lanes, lanesVerified;
//	std::vector<GroundPolyLane<2>> gplanes;
	std::vector<Spline> splines;
	std::vector<Spline_<float>> groundSplines;
	int numWhite;
	int numYellow;
	cv::Mat_<float> yellowImage, whiteImage, roadImage, widthImage;
	cv::Mat_<float> cannyEdgeImage;
	cv::Mat_<float> obstacleImage, fusedYellowImage, fusedWhiteImage;

	//vector<Polynomial<2>> p;
	//vector<double> bestError;

private:
	int nrows, ncols;
	Validator *validator;
	//PulseFilter *laneWidthFilter;
	ColorFilter *laneColorFilter;
	GroundPolyFitter *gpf;
	//ParabolaFitter *fitter;
};

#endif //LANE_DETECTOR_H