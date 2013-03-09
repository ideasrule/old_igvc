#pragma once

#include "cv.h"

// tile images in approx. square configuration
template<typename T>
cv::Mat_<T> tileImages(const std::vector< cv::Mat_<T> >& images)
{
	int N = images.size();
	if(N <= 0) {
		return cv::Mat_<T>();
	}

	int S = (int) ceil(sqrt((double) N));
	S = std::max(S, 1);

	cv::Size sz = images[0].size();
	cv::Mat_<T> out(sz.height*S, sz.width*S);
	out.setTo(0);

	for(int i = 0; i < N; i++) {
		// ensure all images are the right size
		if(images[i].size != images[0].size) {
			break;
		}

		int r = (i / S)*sz.height;
		int c = (i % S)*sz.width;

		cv::Mat dst = out(cv::Rect(c, r, sz.width, sz.height));
		images[i].copyTo(dst);
	}

	return out;
}

