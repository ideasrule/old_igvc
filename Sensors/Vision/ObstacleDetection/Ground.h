#pragma once

#include "Frame.h"

//#include "PerformanceTimer.h"
#include <vector>
#include "RowCol.h"
#include "IObstacleDetector.h"

#include "Memory.h"

using std::vector;
using namespace Pave_Libraries_Camera;

typedef cv::Mat_<double> Vec3D;
/*
#define FULL_IMAGE_WIDTH 640
#define FULL_IMAGE_HEIGHT 480
#define GROUND_SAMPLE_FACTOR 1
#define SF GROUND_SAMPLE_FACTOR
*/
//#define GROUND_SAMPLE_F 1
//#define IMAGE_WIDTH 640
//#define IMAGE_HEIGHT 480
//-readLog C:/IGVC/Logs 2010-06-05-20-26-25

class Ground : public IObstacleDetector
{
public:
    Ground();
    virtual ~Ground();

    virtual bool process(shared_ptr<Frame> frm);

	cv::Point3f **cloud;
	//CPerformanceTimer *intervalTimer

    virtual shared_ptr< cv::Mat_<cv::Vec3b> > overlay(void) {
		return shared_ptr_nodelete(&overlay_);
    }

private:
	cv::Mat_<cv::Vec3b> overlay_;

	// double buffer to avoid excessive copying in estimateGroundPlane
	int *buf1, *buf2;
	bool estimateGroundPlane(
						Frame &frm,
						Vec3D& bestVec3D,
						vector<RowCol>& bestInlierRowCols,
						double& bestScore);
    void cloudToVec3D(int r, int c, Vec3D& vec);
	void getGuessVector(Vec3D& guessVec);

};


#define UCHAR_SAT ((unsigned char)255)
