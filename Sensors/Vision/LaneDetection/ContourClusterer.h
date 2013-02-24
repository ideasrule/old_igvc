#pragma once

#include "Clusterer.h"

#include "OpenCV2BoxFilter.h"

class ContourClusterer : public Clusterer
{
	// If we use OpenMP and make multiple copies of this clusterer,
	// boxFilter will use shallow copies
	const OpenCV2BoxFilter boxFilter;
	cv::Mat_<int> boxFiltered32;
	cv::Mat_<unsigned char> boxFiltered;
	int currSmoothing;
	bool restrictCurrSmoothing;
	std::vector<std::vector<cv::Point> > contours;
	int iContour;

public:

	ContourClusterer(const cv::Mat_<float> &img, const OpenCV2BoxFilter &boxFilter, int singleSmoothing = -1);
	~ContourClusterer();

	shared_ptr<Cluster> emitCluster();

	static const int num_smoothings;

	static OpenCV2BoxFilter makeBoxFilter(const cv::Mat_<unsigned char> &img);
};