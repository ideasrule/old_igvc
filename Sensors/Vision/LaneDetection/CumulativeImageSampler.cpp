#include "CumulativeImageSampler.h"

#include <cstdlib>

CumulativeImageSampler::CumulativeImageSampler(const cv::Mat_<float> &img)
{
	cv::Size size = img.size();
	nrows = size.height;
	ncols = size.width;
	cumsum = new float[nrows * ncols];

	//Calculate cumulative distributition function (for sampling)
	cumsum[0] = img[0][0];
	for(int i = 0; i < nrows * ncols -1; i++)	
	{			
		int r = i / ncols;
		int c = i % ncols;
		float thisImgValue = img[r][c];
		cumsum[i + 1] = cumsum[i] + pow(thisImgValue, 2);
	}
}

CumulativeImageSampler::~CumulativeImageSampler(void)
{
	delete[] cumsum;
}

void CumulativeImageSampler::takeSample(RowCol& rc)
{
	float minRand = 0.0f;
	float maxRand = cumsum[nrows * ncols - 1];
	//Pick a random value in the range of cumsum
	float r = (float) std::rand() / (RAND_MAX + 1.0f);
	float sample = minRand + r * (maxRand - minRand);
	
	//Find where sample lives in the distribution using binary search
	int minIndex = 0;
	int maxIndex = nrows * ncols - 1;	
	while(maxIndex - minIndex > 1)
	{
		int testIndex = (maxIndex + minIndex) / 2;
		float testVal = cumsum[testIndex];
		if(testVal <= sample)
			minIndex = testIndex;
		else
			maxIndex = testIndex;
	}

	rc.row = (int) (minIndex / ncols);
	rc.col = (int) (minIndex % ncols);
}

void CumulativeImageSampler::multiSample(RowCol rcs[], int N)
{
	for(int i = 0; i < N; i++) 
		takeSample(rcs[i]);
}
