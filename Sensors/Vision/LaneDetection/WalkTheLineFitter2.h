#pragma once

#include "Spline.h"
#include "cv.h"
#include <vector>
#include <queue>
#include <map>
#include "Memory.h"
#include "Cluster.h"

class WalkTheLineFitter2 {

public:
	WalkTheLineFitter2(const cv::Mat_<float> &img);
	
	enum VisualizationLevel {NONE, CLUSTER, CLUSTER_SAVE, CIRCLE_POINTS};

	void processCluster(const shared_ptr<Cluster> &cluster, VisualizationLevel visualize = NONE);
	void filterRedundantLanes();
	int appendLanes(std::vector< std::vector<cv::Point> > &out) const;
	int appendSplines(std::vector<Spline> &out) const;

private:

	struct CirclePoint {
		cv::Point point;
		float value;
		cv::Point offCenter;

		bool operator < (const CirclePoint &b);
	};

	int appendClusterCircle(
		const shared_ptr<Cluster> &cluster,
		const cv::Point &center, int radius, 
		std::vector< CirclePoint > &points) const;

	int sortCirclePointsToRuns(
		std::vector<CirclePoint> &points,
		std::vector< std::vector<CirclePoint> > &runs) const;

	struct InternalLane {
		std::vector<cv::Point> ctrl;
		bool redundant;
	};

	std::vector< InternalLane > lanes;
	cv::Mat_<float> img;
	static std::map<const int, shared_ptr<std::vector<cv::Point> > > offsetCache;
};