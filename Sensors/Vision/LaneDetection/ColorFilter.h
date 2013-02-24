#ifndef COLOR_FILTER_H_
#define COLOR_FILTER_H_

#include "cv.h"

using cv::Mat_;
using cv::Vec3b;

class ColorFilter {
public:
	ColorFilter(int nrows, int ncols);
	~ColorFilter();
	//Takes in RGBU8 image. Returns 2 binary float images1x
	void processImage(IplImage * img, IplImage * yellowImage, IplImage * whiteImage, IplImage * roadImage, Mat_<float>& cannyEdgeImage);	
	void processMat(const Mat_<Vec3b> &img, Mat_<float> &yellowImage, Mat_<float> &whiteImage, Mat_<float> &roadImage, Mat_<float> &cannyEdgeImage)
	{
		IplImage cvimg = img, cvy = yellowImage, cvw = whiteImage, cvr = roadImage;
		processImage(&cvimg, &cvy, &cvw, &cvr, cannyEdgeImage);
	}
	IplImage *getHSVImage();

private:
    CvHistogram *vHist;
    CvHistogram *hHist;

    IplImage *hsv;
    IplImage *hSplit;
    IplImage *sSplit;
    IplImage *vSplit;
    IplImage *wTemp;
    IplImage *wTempBright;
    IplImage *wTempShadow;
    IplImage *yTemp;

	//cv::Mat *cannyWhite;
};
#endif  // COLOR_FILTER_H_
