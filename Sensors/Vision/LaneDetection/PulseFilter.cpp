#include "PulseFilter.h"

#define LANE_WIDTH .13 //in m. 13cm = 5.25"
#define STOPLINE_DEPTH .13 //in m
#define STOPLINE_WIDTH 5 //in m

PulseFilter::PulseFilter(int nrows, int ncols) : 
integralImage(nrows+1, ncols+1)
{
}

PulseFilter::~PulseFilter()
{
}

inline int PulseFilter::laneWidthForRow(int row, Camera *cam)
{
	if(row < 1) row = 1;
	double laneWidth = LANE_WIDTH / cam->groundPixelWidthAtRow(row);	
	if(laneWidth < 0)
		laneWidth = 0;
	return laneWidth + 5;
}

void PulseFilter::stoplineDimensionsForRow(int row, int& width, int& depth, Camera *cam)
{
	if(row < 1) row = 1;
	width = STOPLINE_WIDTH / cam->groundPixelWidthAtRow(row);
	if(width < 1) width = 1;
	depth = STOPLINE_DEPTH / cam->groundPixelDepthAtRow(row);
	if(depth < 1) depth = 1;
}

//values are inclusive!
double PulseFilter::rectangleValue(int low, int up, int left, int right)
{
	double A = integralImage[up + 1][right ];
	double B = integralImage[up + 1][left]; 
	double C = integralImage[low][right];
	double D = integralImage[low][left];
	return A - B - C + D;
}

void PulseFilter::applyFilter(const Mat_<float> &monoImage, Mat_<float> &laneWidthImage, Camera *cam)
{
	cv::integral(monoImage, integralImage);
	
	for(int i = 0; i < monoImage.rows; i++) {		
		//Compute filter parameters
		int laneWidth = laneWidthForRow(i, cam);			
		int filterWidth = 3 * laneWidth;		
		//Start of part 1: negative 1s
		int pt1 = -laneWidth / 2 + -laneWidth;
		//start of part 2: positive 2s
		int pt2 = -laneWidth / 2;
		//start of part 3: negative 1s
		int pt3 = (laneWidth + 1) / 2;
		//end
		int pt4 = (laneWidth + 1) / 2 + laneWidth;

		for(int j = -pt1; j < integralImage.cols - pt4 - 2; j++) {
			laneWidthImage[i][j] =  -rectangleValue(i, i, j + pt1, j + pt2) + 2 * rectangleValue(i, i, j + pt2, j + pt3)
				- rectangleValue(i, i, j + pt3, j + pt4 + 1);												
			laneWidthImage[i][j] /= monoImage[i][j] * laneWidth;					
			if(laneWidthImage[i][j] < 0) laneWidthImage[i][j] = 0;
			if(laneWidthImage[i][j] > 1) laneWidthImage[i][j] = 1;

		}		
	}

}

void PulseFilter::applyStoplineFilter(const Mat_<float> &monoImage, const Mat_<float> &laneWidthImage, Mat_<float> &stoplineWidthFilter, Camera *cam)
{
	for(int i = 0; i < monoImage.rows; i++) {
		int stoplineWidth, stoplineDepth;
		stoplineDimensionsForRow(i, stoplineWidth, stoplineDepth, cam);
			
		//compute row bounds
		int row1 = i - stoplineDepth - stoplineDepth / 2;
		if(row1 < 0) row1 = 0;

		int row2 = i - stoplineDepth / 2;
		if(row2 < 0) row2 = 0;

		int row3 = i + stoplineDepth / 2;
		if(row3 >= monoImage.rows) row3 = monoImage.rows - 1;

		int row4 = i + stoplineDepth / 2 + stoplineDepth;
		if(row4 >= monoImage.rows) row4 = monoImage.rows - 1;
		
		double heightPositive = row3 - row2;
		double heightNegative = row4 - row3 + row2 - row1;

		for(int j = stoplineWidth / 2; j < monoImage.cols - stoplineWidth / 2; j++)
		{						
			int col2 = j - stoplineWidth / 2;
			int col3 = j + stoplineWidth / 2;		
			double positive = rectangleValue(row2, row3 - 1, col2, col3);
			double negative = rectangleValue(row1, row2 - 1, col2, col3) +  rectangleValue(row3, row4 - 1, col2, col3);			
			stoplineWidthFilter[i][j] = (positive / heightPositive - negative / heightNegative )/ (double)(col3 - col2);
		}

	}	
}