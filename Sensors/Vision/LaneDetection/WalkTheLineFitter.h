#pragma once

#include "Spline.h"
#include "cv.h"
#include <vector>
#include <list>
#include "Frame.h"

#include "Memory.h"

class WalkTheLineFitter {

public:
	WalkTheLineFitter(const cv::Mat_<float> &img);
	int appendAllFits(std::vector<Spline> &out);
	
private:

	struct CirclePoint {
		cv::Point point;
		float value;
		cv::Point offCenter;

		bool operator < (const CirclePoint &b);
	};

	/** A cluster of points from a possibly smoothed image. 
	Stored as a smaller matrix, with an offset into the whole */
	class Cluster {

	public:

		int smoothingKernelSize;
		cv::Mat_<float> mat;
		cv::Point offset;
		std::list<cv::Point> seeds; // unique queries into the cache
		cv::Point deepest;
		int maxDepth;

		Cluster(const cv::Mat_<float> &img, cv::Point start);

		// global point, with offset added
		bool containsPoint(const cv::Point &pt) const;

		// local pixels, add offset to make image pixels
		int appendElementsOnCircle(
			const cv::Point &center,
			int radius,
			std::vector<CirclePoint> &points) const;

		int appendRunsOnCircle(
			const cv::Point &center,
			int radius,
			std::vector<std::vector<CirclePoint> > &runs) const;

	private:

		struct FloodFillScratchpad; // defined in cpp file
		void floodFill(cv::Point center, FloodFillScratchpad* scratchpad);
	};

	typedef shared_ptr<Cluster> ClusterPtr;

	/** A possibly smoothed version of the original image */
	class FilteredImg {

	public:

		const int smoothingKernelSize; // 0 if no smoothing
		std::vector<float> cumsum;
		cv::Mat_<float> filtImg;

		FilteredImg(int kernelSize, const cv::Mat_<float> &img);
		~FilteredImg() {};

		/**
		At a random nonzero point in the smoothed image, flood fill using a DFS
		and return a matrix with only those points. The out matrix will be smaller
		than the original, and will be located at offsetOut within the original.
		deepestOut is the point (w/r/t the out matrix) that was visited
		by the deepest iteration of the DFS.
		*/
		ClusterPtr findCluster(const cv::Point &pt); // method is not const due to caching

	private:

		std::vector<ClusterPtr> clusterCache; // TODO: better data structure
		// although a linear search through this is faster iff clusters have many points
	};

	typedef shared_ptr<FilteredImg> FilteredImgPtr;

	std::vector<FilteredImgPtr> filtImgs;

	// http://en.wikipedia.org/wiki/Ramer-Douglas-Peucker_algorithm
	// this is recursive
	static void simplifyPointList(std::list<cv::Point> &pts, float epsilon);
	static double sampleRadius(double mean, double variance);

public:
	void showClusterDemo();
	static void clusterDemoTrackbarCallback(int, void*);
	static void clusterDemoMouseCallback(int, int, int, int, void*);
	int clusterDemoTrackbarPos;
};
