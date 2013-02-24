#pragma once

#include "cv.h"
#include <vector>
#include <set>
#include "Memory.h"

#define CLUSTER_USE_FLANN

class Cluster {

public:
	class PointCompare {
	public:
		bool operator() (const cv::Point &a, const cv::Point &b) const;
	};

public:
	const int smoothing;

	const cv::Rect boundsInWhole;
	const cv::Point offset;

	const int count;
	std::vector<cv::Point> deepest;

	Cluster(int count,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0);

	virtual ~Cluster();

	// global point, with offset added
	virtual bool containsPoint(const cv::Point &pt) const = 0;

	virtual void drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const = 0;
};

class MatCluster : public Cluster {

	cv::Mat_<unsigned char> mat;

public:
	MatCluster(const std::vector<cv::Point> &points,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0);
	MatCluster(const cv::Mat_<unsigned char> &wholeOrPart,
		int count,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0, bool copy = false, bool matIsWhole = true);
	~MatCluster();

	bool containsPoint(const cv::Point &pt) const;
	void drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const;
};


class FlannCluster : public Cluster {

	shared_ptr< cv::flann::Index_<float> > pointIndex;
	cv::Mat_<float> pointMat;

public:
	FlannCluster(const std::vector<cv::Point> &points,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0);
	~FlannCluster();

	bool containsPoint(const cv::Point &pt) const;
	void drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const;
};

class PointSetCluster : public Cluster {

	std::set<cv::Point, PointCompare> pointSet;

public:
	PointSetCluster(const std::vector<cv::Point> &points,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0);
	~PointSetCluster();

	bool containsPoint(const cv::Point &pt) const;
	void drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const;
};

class ContourCluster : public Cluster {

	cv::Mat contourMat;

public:
	ContourCluster(const std::vector<cv::Point> &contour,
		int count,
		const cv::Rect &boundsInWhole,
		std::vector<cv::Point> deepest,
		int smoothing = 0);
	~ContourCluster();

	bool containsPoint(const cv::Point &pt) const;
	void drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const;
};