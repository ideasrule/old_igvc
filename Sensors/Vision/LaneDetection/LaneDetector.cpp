
#include "LaneDetector.h"
#include "SplineFitter.h"
#include "WalkTheLineFitter2.h"
#include "highgui.h"
#include "OvershootClusterer.h"
#include "IntegralClusterer.h"
#include "ContourClusterer.h"

LaneDetector::LaneDetector(int nrows, int ncols) : 
	nrows(nrows), ncols(ncols),
	numYellow(2), numWhite(2),
	yellowImage(nrows, ncols), whiteImage(nrows, ncols),
	roadImage(nrows, ncols), widthImage(nrows, ncols),
	obstacleImage(nrows, ncols),
	fusedYellowImage(nrows, ncols), fusedWhiteImage(nrows/2, ncols/2),
	splines(), groundSplines()
{
	laneColorFilter = new ColorFilter(nrows, ncols);
	validator = new Validator();
	gpf = new GroundPolyFitter(nrows, ncols);
	//laneWidthFilter = new PulseFilter(nrows, ncols);
}

LaneDetector::~LaneDetector()
{
	clear();
	delete laneColorFilter;
	//delete objectiveImgYellow;
	//delete objectiveImgWhite;
	delete validator;
	delete gpf;
}

void LaneDetector::clear()
{
	splines.clear();
	groundSplines.clear();
}

bool LaneDetector::process(shared_ptr<Frame> frm, VisionTiming &timing)
{
	clear();

	if (frm->width != ncols || frm->height != nrows) {
		cout << "Lane detection not configured for "
			<< frm->width << "x" << frm->height 
			<< " frames!" << endl;
		return false;
	}

	timing.start(TIMER_LANE_COLOR_FILTER);
	laneColorFilter->processMat(frm->color, yellowImage, whiteImage, roadImage, cannyEdgeImage);

	timing.end(TIMER_LANE_COLOR_FILTER);

	cv::imshow("White Image", whiteImage);

//	timing.start(TIMER_LANE_WIDTH_FILTER);
//	laneWidthFilter->applyFilter(frm.mono, widthImage, frm.camera);
//	timing.end(TIMER_LANE_WIDTH_FILTER);

	timing.start(TIMER_FUSION);

	//Prepare obstacle image
	//obstacleImage.setTo(0.0f);
	//cv::resize(cannyEdgeImage, fusedWhiteImage, fusedWhiteImage.size());
	cannyEdgeImage.copyTo(fusedWhiteImage);
	/*
	for (int r = 0; r < obstacleImage.rows; r++)
		for (int c = 0; c < obstacleImage.cols; c++)
		{
			if (frm->obstacle[r][c] || !frm->validArr[r][c])
			{
				obstacleImage[r][c] = 1.0f;
				fusedWhiteImage[r][c] = 0.0f;
			}
		}
		*/

	//cv::imshow("ObstacleImagePreBlur", obstacleImage);

	//cv::boxFilter(obstacleImage, obstacleImage, obstacleImage.depth(), cv::Size(5,5), 
	//	Point(-1,-1) /*center anchor*/, false /*don't normalize*/);
	//cv::GaussianBlur(obstacleImage, obstacleImage, cv::Size(25,25), 0 /*calculate sigma*/);

	//cv::imshow("ObstacleImagePreFlip", obstacleImage);

	//Invert obstacle image
	//for (int r = 0; r < obstacleImage.rows; r++)
	//	for (int c = 0; c < obstacleImage.cols; c++)
	//		obstacleImage[r][c] = std::min(1.0f, 1.0f-obstacleImage[r][c] + (frm->validArr[r][c] ? 0.0f : 1.0f));


	
	//cv::multiply(cannyEdgeImage, obstacleImage, fusedWhiteImage);

	//cv::multiply(cannyEdgeImage, whiteImage, fusedWhiteImage);
	cv::imshow("FusedWhiteImage", fusedWhiteImage);

	//FUSION TIME!!!!!!
	//if (numWhite)
	//{
	//	//fusedWhiteImage = whiteImage.mul(widthImage).mul(obstacleImage);
	//	cv::multiply(whiteImage, widthImage, fusedWhiteImage);
	//	cv::multiply(fusedWhiteImage, obstacleImage, fusedWhiteImage);
	//}

//
//	if (numYellow)
//	{
//		//fusedYellowImage = yellowImage.mul(widthImage).mul(obstacleImage);
//		cv::multiply(yellowImage, widthImage, fusedYellowImage);
//		cv::multiply(fusedYellowImage, obstacleImage, fusedYellowImage);
//	}
//
//	//Stopline stopline = slFitter->processImage(whiteImage, stoplineWidthFilter);
	timing.end(TIMER_FUSION);

	timing.start(TIMER_PARABOLA_FIT);
	{
		WalkTheLineFitter2 wtlf(fusedWhiteImage);
		
		shared_ptr<Clusterer> clusterer(
			new ContourClusterer(fusedWhiteImage,
			ContourClusterer::makeBoxFilter(fusedWhiteImage > 0)));
		clusterer->showDebugWindow("Clusterer Debug");
		shared_ptr<Cluster> c;
		while (c = clusterer->emitCluster())
		{
			wtlf.processCluster(c, WalkTheLineFitter2::VisualizationLevel::NONE);
			//wtlf.processCluster(c, true);
			//waitKey(50);
		}

		wtlf.appendSplines(splines);

		/* If we're doing a lower-resolution view
		for (vector<Spline>::iterator it = splines.begin(), itEnd = splines.end();
			it != itEnd; ++it)
		{
			for (vector<Point>::iterator ptIt = it->controlPoints.begin(),
				ptItEnd = it->controlPoints.end(); ptIt != ptItEnd; ++ptIt)
			{
				*ptIt *= 2;
			}
		}
		*/
	}

	timing.end(TIMER_PARABOLA_FIT);

	return splines.size() > 0;
	//return (numLanesFound > 0);
}