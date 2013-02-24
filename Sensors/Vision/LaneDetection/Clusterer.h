#pragma once

#include "Cluster.h"
#include "cv.h"
#include <vector>
#include <queue>

class Clusterer {

protected:

	cv::Mat_<float> img;
	
public:

	Clusterer(const cv::Mat_<float> &img) : img(img) {}
	virtual ~Clusterer() {}

	// note: shared_ptr may be set to 0
	virtual shared_ptr<Cluster> emitCluster() = 0;

	virtual void showDebugWindow(const char *windowName) {};
};