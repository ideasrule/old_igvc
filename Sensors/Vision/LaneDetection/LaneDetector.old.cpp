
#include "LaneDetector.h"
#include "ParabolaFitter.h"
#include "ColorFilter.h"
#include "PulseFilter.h"
#include "Validator.h"


LaneDetector::LaneDetector(int nrows, int ncols) : 
nrows(nrows), ncols(ncols),
numYellow(2), numWhite(2),
yellowImage(nrows, ncols), whiteImage(nrows, ncols),
roadImage(nrows, ncols), widthImage(nrows, ncols),
obstacleImage(nrows, ncols),
fusedYellowImage(nrows, ncols), fusedWhiteImage(nrows, ncols)
//stoplineWidthFilter(nrows, ncols)
{
	laneColorFilter = new ColorFilter(nrows, ncols);
	fitter = new ParabolaFitter(nrows, ncols);
	// Stopline code commented out
	//StoplineFitter * slFitter = new StoplineFitter(nrows, ncols, tform);
	validator = new Validator();
	laneWidthFilter = new PulseFilter(nrows, ncols);
}

LaneDetector::~LaneDetector()
{
	clear();
	delete laneColorFilter;
	//delete objectiveImgYellow;
	//delete objectiveImgWhite;
	//delete fitter;
	delete validator;
	delete laneWidthFilter;
}

void LaneDetector::clear()
{
	for (std::vector<ImageLane *>::iterator it = lanes.begin();
		it < lanes.end(); it++ )
	{
		delete *it;
	}
	lanes.clear();
	lanesVerified.clear();
}

bool LaneDetector::process(const Frame &frm, VisionTiming &timing)
{
	clear();

//	timing.start(TIMER_LANE_COLOR_FILTER);
	laneColorFilter->processMat(frm.color, yellowImage, whiteImage, roadImage, cannyEdgeImage);

//	timing.end(TIMER_LANE_COLOR_FILTER);

//	timing.start(TIMER_LANE_WIDTH_FILTER);
//	laneWidthFilter->applyFilter(frm.mono, widthImage, frm.camera);
//	timing.end(TIMER_LANE_WIDTH_FILTER);

//	timing.start(TIMER_FUSION);

	//Prepare obstacle image
//	for (int r = 0; r < obstacleImage.rows; r++)
//		for (int c = 0; c < obstacleImage.cols; c++)
//			obstacleImage[r][c] = frm.obstacle[r][c] > 0 ? 1.0f : 0.0f;
//	cv::boxFilter(obstacleImage, obstacleImage, -1 /*wtf?*/, cv::Size(5,5), 
//		Point(-1,-1) /*center anchor*/, false /*don't normalize*/);
//	cv::GaussianBlur(obstacleImage, obstacleImage, cv::Size(9,9), 0 /*calculate sigma*/);
//
//	//FUSION TIME!!!!!!
//	if (numWhite)
//	{
//		//fusedWhiteImage = whiteImage.mul(widthImage).mul(obstacleImage);
//		cv::multiply(whiteImage, widthImage, fusedWhiteImage);
//		cv::multiply(fusedWhiteImage, obstacleImage, fusedWhiteImage);
//	}
//
//	if (numYellow)
//	{
//		//fusedYellowImage = yellowImage.mul(widthImage).mul(obstacleImage);
//		cv::multiply(yellowImage, widthImage, fusedYellowImage);
//		cv::multiply(fusedYellowImage, obstacleImage, fusedYellowImage);
//	}
//
//	//Stopline stopline = slFitter->processImage(whiteImage, stoplineWidthFilter);
////	timing.end(TIMER_FUSION);
//
////	timing.start(TIMER_PARABOLA_FIT);
//
//	std::vector<ImageLane *> lanesWhite, lanesYellow;
//
//	fitter->findFit(fusedWhiteImage, 600, numWhite, lanesWhite);			
//	fitter->findFit(fusedYellowImage, 600, numYellow, lanesYellow);	
//
//	for(int i = 0; i < lanesWhite.size(); i++)
//	{
//		lanesWhite[i]->color = WHITE;
//		lanes.push_back(lanesWhite[i]);
//	}
//
//	for(int i = 0; i < lanesYellow.size(); i++) 
//	{
//		lanesYellow[i]->color = YELLOW;
//		lanes.push_back(lanesYellow[i]);
//	}
//
//	validator->validateLanes(lanes, lanesVerified);
//	
////	timing.end(TIMER_PARABOLA_FIT);
//
	return lanesVerified.size() > 0;
}