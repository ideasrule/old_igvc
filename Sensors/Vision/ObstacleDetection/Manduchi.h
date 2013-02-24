/**********************************************************************
 How to tune obstacle detection
 -Manduchi: detection slope, 
            Hmin/Hmax at difference distances, 
            min compatible point for classifying as obstacle
			detection max distance threshold
 -Clustering: cluster size threshold, 
              cluster avg slope threshold
 **********************************************************************/

// if this is #pragma once, then for the 640 hack, both Manduchi.h
// and Manduchi640.h are included, we just want one included,
// so it's defined like this in both files
#ifndef MANDUCHI_H
#define MANDUCHI_H

#include "Frame.h"
#include "IObstacleDetector.h"
#include "Memory.h"

#include <vector>

using namespace Pave_Libraries_Camera;

typedef cv::Mat_<double> Vec3D;
#define UCHAR_SAT ((unsigned char)255)

class Manduchi : public IObstacleDetector
{
public:
    Manduchi();
	virtual ~Manduchi();
    virtual bool process(shared_ptr<Frame> frm);

	//CPerformanceTimer *intervalTimer;

    virtual shared_ptr< cv::Mat_<cv::Vec3b> > overlay(void) {
		return shared_ptr_nodelete(&overlay_);
    }

private:
	cv::Mat_<cv::Vec3b> overlay_;
	
	std::vector<float*> depthTileA, depthTileB, depthTileC, depthTileD;

	// obstacle labels for clustering
	int *labelMap;
	
	// set of obstacles
	vector<vector<cv::Point2i>> clusteredObstacles;
	vector<int> relabelList;

	// for disjoint_set in the findConnectedComponent() function
	std::vector<int> rank;
	std::vector<int> parent;

	void blockACProcess(float *depthMap);
	void blockBDProcess(float *depthMap);
	void checkCompatibility(float *p1, float *p2, float Hmin2, float Hmax2);
	void assignWindowSize(float *p1, float& Hmin, float& Hmax, float& searchMin, float& searchMax, float& searchMaxt2, float& searchBase);
	void findConnectedComponent(unsigned char *img, int *labelMap, float *cloud);

	void simpleRailingDetector(cv::Mat_<cv::Point3f> depthMap, bool *validArr, bool *result);   

};

#endif // MANDUCHI_H