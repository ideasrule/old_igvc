#ifndef CUMULATIVE_IMAGE_SAMPLER_H
#define CUMULATIVE_IMAGE_SAMPLER_H

#include "cv.h"
#include "RowCol.h"

class CumulativeImageSampler
{
public:
	CumulativeImageSampler(const cv::Mat_<float> &img);
	~CumulativeImageSampler(void);

	void takeSample(RowCol &rc);
	void multiSample(RowCol rcs[], int N);

private:
	float *cumsum;
	int nrows, ncols;
};

#endif // CUMULATIVE_IMAGE_SAMPLER_H