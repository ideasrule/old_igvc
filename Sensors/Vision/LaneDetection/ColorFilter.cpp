#include <cmath>
#include <algorithm>
#include "highgui.h"
#include <stdio.h>
#include "ColorFilter.h"

// Constants for yellow filter
#define MIN_YELLOW_HUE 25
#define MAX_YELLOW_HUE 32
#define YELLOW_RANGE 15
#define MIN_YELLOW_SAT 60

// Constants for white filter
#define MAX_WHITE_SAT 60

// histogram constants
#define H_HIST_SIZE 180
#define V_HIST_SIZE 255

using namespace std;

float range_0[]={0,256};
float* ranges[] = { range_0 };

ColorFilter::ColorFilter(int nrows, int ncols)
{	
	hsv         = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 3);
	hSplit      = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
	sSplit      = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
	vSplit	    = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
    yTemp       = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
    wTempBright = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
    wTempShadow = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
    wTemp       = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
///	cannyWhite  = cvCreateImage(cvSize(ncols, nrows), IPL_DEPTH_8U, 1);

    int histSize = H_HIST_SIZE;
	hHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1);
    histSize = V_HIST_SIZE;
	vHist = cvCreateHist(1, &histSize, CV_HIST_ARRAY, ranges, 1);
}

void ColorFilter::processImage(IplImage * img, IplImage *yellowImage, IplImage *whiteImage, IplImage * roadImage, Mat_<float>& cannyEdgeImage)
{
	if(img == NULL) {
		printf("\nInvalid input");	
		return;
	}
	
	//start new purple filter
	//IplImage *tempRedImage  = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);	
	//IplImage *tempGreenImage  = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);	
	//IplImage *tempBlueImage  = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);	
	//IplImage *tempImage  = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 3);	
	//IplImage *tempImage2  = cvCreateImage(cvSize(img->width,img->height), IPL_DEPTH_8U, 1);	

	//cvSplit(img,tempRedImage,tempGreenImage,tempBlueImage,0);

	//cvSetImageCOI(tempImage,1);
	//cvCopy(tempRedImage,tempImage);
	//cvSetImageCOI(tempImage,2);
	//cvCopy(tempBlueImage,tempImage);
	//cvSetImageCOI(tempImage,0);
	//cv::imshow("color", img);
	//cvCvtColor(img,tempImage2,CV_RGB2GRAY);

//*************************************************************************
// IGVC 2010
// A different approach... Use canny edge filter to find edges, then check
// if anything near edges have the correct color characteristics
// (i.e. near zero saturation and relatively high intensity)

	//cv::Mat temp(tempImage2);
	//// histogram equalization
	//vector<int> histBins(256, 0);
	//for (int r = 0; r < temp.rows; ++r) 
	//	for (int c = 0; c < temp.cols; ++c) 
	//		histBins[temp.at<unsigned char>(r, c)]++;
	//	
	//int i = 255;
	//int sum = 0;
	//while (i > 150 && sum < 5000) {
	//	sum += histBins[i--];
	//}
	//i+=2;  //take the next highest one and all those after and set that as max intensity
	//if (i > 255) i = 255;
	//double scaleFactor = (255.0 / (double)i);
	//temp = temp * scaleFactor;



	//cv::GaussianBlur(temp, temp, cv::Size(3, 3), 5, 5);
	//cv::imshow("bw", temp);
	//cv::Canny(temp, cannyEdgeImage, 1, 140);  //1, 120
	//cv::Canny(temp, cannyEdgeImage, 1, 160);  //1, 120 //70, 140
	//cv::boxFilter(cannyEdgeImage, cannyEdgeImage, CV_8U, cv::Size(15, 15),
	//	cv::Point(-1, -1), false);
	//cv::imshow("edge", cannyEdgeImage);
//**************************************************************************

	//cvThreshold(tempImage2, tempImage2, 215, 255,CV_THRESH_BINARY);
	//cvScale(tempImage2,yellowImage);

	////end new purple filter

	//// Create HSV image from original, and split into H, S, and V channels.
	//cv::Mat img2(img,true);
	//cv::Mat hsv2(480,640,CV_8UC3);
	/*
	for(int i=0; i < 640*480; i++) {
		CvScalar s;
		s=cvGet2D(img,i/640,i%640);
		s.val[0]=0;
		cvSet2D(img,i/640,i%640,s);
	}
	cv::imshow("img",img);*/
	//imshow("img2",img2);
	//cvtColor(img2,hsv2,CV_BGR2HSV);
	cvCvtColor(img, hsv, CV_BGR2HSV);		
	cvSplit(hsv, hSplit, sSplit, vSplit, NULL);
 //   
	//// BEGIN NEW YELLOW FILTER

 //   // Calculate histogram of hue image and find the max value
	//cvCalcHist(&hSplit, hHist, 0, NULL);
	//int maxYellowHue = 0;
	//int maxBinVal = 0;

	///*for (int i = MIN_YELLOW_HUE; i < MAX_YELLOW_HUE; i ++)
	//{
	//	int binVal = cvQueryHistValue_1D(hHist, i);
	//	if (binVal > maxBinVal)
	//	{
	//		maxYellowHue = i;
	//		maxBinVal    = binVal;
	//	}
	//}*/
 //   // Threshold above and below max value
 //   //cvThreshold(hSplit, yTemp, maxYellowHue + YELLOW_RANGE, 0, CV_THRESH_TOZERO_INV);
 //   //cvThreshold(yTemp,  yTemp, maxYellowHue - YELLOW_RANGE, 1, CV_THRESH_BINARY);
 //   //
 //   //// Apply low-saturation threshold
 //   //cvMul(sSplit, yTemp, yTemp);
 //   //cvThreshold(yTemp, yTemp, MIN_YELLOW_SAT, 1, CV_THRESH_BINARY);
 //   //cvMul(vSplit, yTemp, yTemp);
 //   //cvScale(yTemp, yellowImage, 1.0, 0);

 //   //// END NEW YELLOW FILTER

 //   // BEGIN WHITE FILTER

	//// IGVC Hack

	//// Hacked for ramp case
	//cvThreshold(hSplit, wTempBright, 10, 1, CV_THRESH_BINARY_INV);

	//// remove yellow and green from hue
	//cvThreshold(hSplit, wTemp, 100, 1, CV_THRESH_BINARY);

	//// Combine with value channel
	//cvAdd(wTemp, wTempBright, wTemp);
	//cvThreshold(wTemp, wTemp, 1, 1, CV_THRESH_TRUNC);
	//cvMul(wTemp, vSplit, wTemp);

	//// Caclulate histogram and find max value
 //   cvCalcHist(&wTemp, vHist, 0, NULL);
	//int maxWhiteBin = 0;
	//maxBinVal = 0;
 //   for (int i = 1; i < V_HIST_SIZE; i ++)
 //   {
 //       int binVal = cvQueryHistValue_1D(vHist, i);
 //       if (binVal > maxBinVal)
 //       {
 //           maxWhiteBin = i;
 //           maxBinVal   = binVal;
 //       }
 //   }

 //   // Threshold below max value
 //   if (maxWhiteBin == 255)
 //       maxWhiteBin = 254;

 //   cvThreshold(wTemp, wTemp, 200, 0, CV_THRESH_TOZERO);

	//// END IGVC Hack

 ////   // Apply a low-saturation threshold
	////cvThreshold(sSplit, wTempShadow, MAX_WHITE_SAT, 1, CV_THRESH_BINARY);
 ////   // Combine with hue and filter out hues through the green range
 ////   cvMul(hSplit, wTempShadow, wTempShadow);
 ////   cvThreshold(wTempShadow, wTempShadow, 90, 1, CV_THRESH_BINARY);

 ////   cvMul(vSplit, wTempShadow, wTempShadow);
 ////   // Threshold below max value
 ////   cvThreshold(wTempShadow, wTempShadow, 40, 0, CV_THRESH_TOZERO);
 ////   cvShowImage("White Fltr Shadow", wTempShadow);

 ////   // Apply high-saturation threshold
 ////   cvThreshold(sSplit, wTempBright, MAX_WHITE_SAT, 1, CV_THRESH_BINARY_INV);
 ////   // Combine with hue and threshold all values greater than yellow to zero
 ////   cvMul(hSplit, wTempBright, wTempBright);    
 ////   cvThreshold(wTempBright, wTempBright, 30, 1, CV_THRESH_BINARY_INV);
 ////   // Combine with value channel and remove yellow lanes
 ////   cvMul(vSplit, wTempBright, wTempBright);
 ////   cvThreshold(yTemp,  yTemp, 0, 1, CV_THRESH_BINARY_INV);
 ////   cvMul(wTempBright, yTemp, wTempBright);
 ////   // Caclulate histogram and find max value
 ////   cvCalcHist(&wTempBright, vHist, 0, NULL);
	////int maxWhiteBin = 0;
	////maxBinVal = 0;
 ////   for (int i = 128; i < V_HIST_SIZE; i ++)
 ////   {
 ////       int binVal = cvQueryHistValue_1D(vHist, i);
 ////       if (binVal > maxBinVal)
 ////       {
 ////           maxWhiteBin = i;
 ////           maxBinVal   = binVal;
 ////       }
 ////   }

 ////   // Threshold below max value
 ////   if (maxWhiteBin == 255)
 ////       maxWhiteBin = 254;
 ////   cvThreshold(wTempBright, wTempBright, maxWhiteBin, 0, CV_THRESH_TOZERO);
 ////   //cvShowImage("White Fltr Bright", wTempBright);

 ////   cvAdd(wTempShadow, wTempBright, wTemp);


	//cvSmooth(wTemp,wTemp,CV_MEDIAN, 5, 0);
 //   cvScale(wTemp, whiteImage, 1.0, 0);

	// Trying a white filter which is a green-white contrast
	// Assume that the median hue is green/yellow since the grass is dead
	// But median is hard so we'll just go with the average
	// Then get a spectrum between that and white
	

 //   // END WHITE FILTER

//***************************************************************************
//identify white by high brightness and low saturation
	cv::Mat valueMask(480, 640, vSplit->depth);
	cvEqualizeHist(vSplit, vSplit);
	cv::threshold(vSplit, valueMask, 200, 255, cv::THRESH_BINARY);
	//cv::imshow("value", valueMask);
	cv::Mat saturationMask(480, 640, sSplit->depth);
	cv::threshold(sSplit, saturationMask, 85, 255, cv::THRESH_BINARY_INV);
	//cv::imshow("sat", saturationMask);
	cannyEdgeImage = valueMask.mul(saturationMask);
	//cv::imshow("white", cannyEdgeImage);
	//cannyEdgeImage = cannyEdgeImage.mul(cannyEdgeImage);
	float high=0;
	float low=1;
	for(int i=0; i < 640*480; i++) {
		CvScalar s1,s2,s3;
		//s=cvGet2D(img,i/640,i%640);
		//printf("%d\n",(int)s.val[1]);

		//float green_prop=s.val[1]/(s.val[0]+s.val[1]+s.val[2]);
		//if (green_prop<low) low=green_prop;
		//if (green_prop>high) high=green_prop;
		//if (green_prop>0.4) cannyEdgeImage.at<float>(i/640,i%640)=0;
		s1=cvGet2D(hSplit,i/640,i%640);
		s2=cvGet2D(vSplit,i/640,i%640);
		s3=cvGet2D(sSplit,i/640,i%640);
		if (s1.val[0]>72) cannyEdgeImage.at<float>(i/640,i%640)=0;
//		printf("%d %d %d\n",(int)s1.val[0],(int)s2.val[0],(int)s3.val[0]);
		//s.val[1]=s.val[1]/2.0;
		//cvSet2D(img,i/640,i%640,s);
	}
	printf("Low and high are %f %f\n",low,high);
	cv::medianBlur(cannyEdgeImage, cannyEdgeImage, 3);
	
	//cv::imshow("combined filter", cannyEdgeImage);

//***************************************************************************

	//cvReleaseImage(&tempImage2);
}

IplImage * ColorFilter::getHSVImage()
{
	return hsv;
}

ColorFilter::~ColorFilter()
{
    cvReleaseImage(&hsv);
    cvReleaseImage(&hSplit);
    cvReleaseImage(&sSplit);
    cvReleaseImage(&vSplit);

    cvReleaseImage(&wTempBright);
    cvReleaseImage(&wTempShadow);
    cvReleaseImage(&yTemp);

	cvReleaseHist(&vHist);
    cvReleaseHist(&hHist);
}
