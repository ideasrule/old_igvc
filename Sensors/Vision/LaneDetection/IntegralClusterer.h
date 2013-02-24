#pragma once

#include "Clusterer.h"
#include <boost/circular_buffer.hpp>

class IntegralClusterer : public Clusterer
{
	const cv::Mat_<int> sum;
	cv::Point curr;
	int currSmoothing;
	bool restrictCurrSmoothing;
	std::vector<cv::Mat_<unsigned char> > markings;
	boost::circular_buffer<cv::Point> floodFillQueue;

	inline bool sumHasPoint(const int &x, const int &y, const int &smoothFactor) const
	{
		if (smoothFactor == 0) return img(y,x) > 0;
		else
		{
			int ytop = max(0, y-smoothFactor);
			int ybottom = min(sum.rows-1, y+smoothFactor);
			int xleft = max(0, x-smoothFactor);
			int xright = min(sum.cols-1, x+smoothFactor);

			int integralVal =
				sum(ybottom,xright) - sum(ybottom,xleft)
				- sum(ytop,xright) + sum(ytop,xleft);
			return (integralVal > 0);
		}
	}

public:

	IntegralClusterer(const cv::Mat_<float> &img, const cv::Mat_<int> &integral, int singleSmoothing = -1);
	~IntegralClusterer();

	shared_ptr<Cluster> emitCluster();

	static const int num_smoothings;
};