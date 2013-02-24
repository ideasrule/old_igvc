#pragma once

#include "Clusterer.h"

class OvershootClusterer : public Clusterer
{

public:

	OvershootClusterer(const cv::Mat_<float> &img);
	~OvershootClusterer();

	shared_ptr<Cluster> emitCluster();

	void drawTo(cv::Mat_<cv::Vec3b> &out) const;

	// Will be updated on each new cluster emitted;
	// will be deleted when no clusters are left
	void showDebugWindow(const char *windowName);

	static void debugWindowMouseCallback(int,int,int,int,void*);

private:

	const char *debugWindowName;
	int debugX, debugY;
	shared_ptr<Cluster> debugCluster;

	void renderDebug() const;
	cv::Mat_<unsigned char> smallestOvershootVisit;

	struct FloodFillArgs
	{
		cv::Point pt;
		unsigned char overshoot; // 10010 has overshoot 1
		unsigned char maxOvershoot; // 10010 has max overshoot 2
	};

	void floodFill(FloodFillArgs args);
	
	struct Temporary {
		//cv::Mat_<unsigned char> marking;
		cv::Rect bounds;
		std::vector<cv::Point> points;
		cv::Point deepest;
		int count;
		bool emitsClusters;
		std::queue<FloodFillArgs> floodFillQueue;
	};
	std::vector<Temporary> temps;

	int currRow, currCol;
	std::queue< shared_ptr<Cluster> > recentClusters;
};