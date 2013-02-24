#include "StoplineFitter.h"
#include <float.h>
#include "Image.h"


StoplineFitter::StoplineFitter(int nrows, int ncols, Transform * tform)
{
	this->tform = tform;
	fused = cvCreateImage( cvSize(ncols, nrows), IPL_DEPTH_32F, 1);	
	fusedBinary = cvCreateImage( cvSize(ncols, nrows), IPL_DEPTH_8U, 1);
}

//scores comp according to fused
double StoplineFitter::scoreConnectedComponent(CvConnectedComp& comp)
{
	cvSetImageROI(fused, comp.rect);			
	CvScalar sum = cvSum(fused);
	cvResetImageROI(fused);
	return sum.val[0] / sqrt((double)(comp.rect.height * comp.rect.width));	
}

Stopline StoplineFitter::processImage(IplImage *whiteFilter, IplImage *pulseFilter)
{			
	cvMul(whiteFilter, pulseFilter, fused);
	cvThreshold(fused, fusedBinary, .1, 1, CV_THRESH_BINARY);

	BwImage fusedBinaryData = BwImage(fusedBinary);
	
	//component numbers start at 2, since 1 means "unlabeled" and 0 means "not a component"
	int currComponentNumber = 2;
	CvConnectedComp bestComp;
	double bestCompScore = -DBL_MAX;

	for(int i = 0; i < fused->height; i++) {
		for(int j = 0; j < fused->width; j++)
		{
			unsigned char thisData = fusedBinaryData[i][j];
			if(fusedBinaryData[i][j] == 1)
			{
				CvConnectedComp currComp;
				cvFloodFill(fusedBinary, cvPoint(j, i), cvScalarAll(currComponentNumber), cvScalarAll(0), cvScalarAll(0), &currComp, 8);				
				double thisScore = scoreConnectedComponent(currComp);
				if(thisScore > bestCompScore) {
					bestCompScore = thisScore;
					bestComp = currComp;
				}
				currComponentNumber++;				
			}
		}
	}
	
	if( bestCompScore > 0) 
	{
		int centerRow = bestComp.rect.y + bestComp.rect.height / 2;
		int centerCol = bestComp.rect.x + bestComp.rect.width  / 2;
		double x, z;
		tform->pixelsToGround(centerRow, centerCol, x, z);
		bool isValid = bestCompScore > 2.0;
		return Stopline_new(bestCompScore, bestComp.rect, x, z, isValid);
	}
	else
	{
		return Stopline_new(-1, cvRect(0, 0, 0, 0), 0, 0, false);
	}
}

void StoplineFitter::drawStopline(IplImage * img, Stopline stopline)
{
	CvRect rect = stopline.rect;
	cvRectangle(img, cvPoint(rect.x, rect.y), cvPoint(rect.x + rect.width, rect.y + rect.height), cvScalar(255, 0, 0), 1);
	char text[10];
	sprintf(text, "%2.2f m", stopline.z);
	int textrow = stopline.rect.y - 10;
	if(textrow < 10) textrow = 10;
	int textcol = stopline.rect.x + stopline.rect.width / 2 - 40;
	if(textcol < 5) textcol = 5;
	if(textcol > 550) textcol = 550;
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, 1.0, 0, 2);
	cvPutText(img, text, cvPoint(textcol, textrow), &font, cvScalar(255, 200, 100));
}