#pragma once
#include <cxcore.h>
#include <cv.h>
#include "Stopline.h"
#include "Transform.h"


class StoplineFitter
{
public:
	StoplineFitter(int nrows, int ncols, Transform * tform);
	Stopline processImage(IplImage * whiteFilter, IplImage * pulseFilter);
	void drawStopline(IplImage * img, Stopline stopline);
	IplImage * fused;
	IplImage * fusedBinary;
private:
	double scoreConnectedComponent(CvConnectedComp& comp);
	Transform * tform;
};