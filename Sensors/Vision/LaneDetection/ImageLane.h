#ifndef LANE_H
#define LANE_H

#include <vector>
//#include "Frame.h"
#include "gendefs.h"
//#include "LaneColor.h"
#include "Parabola.h"
//#include "Transform.h"
//#include "ImageCloud.h"
#include "Point2D.h"
#include "cv.h"

using namespace cv;
using namespace std;

class ImageLane {
public:
	inline int operator[](const int rowIndx) {
		return cols[rowIndx];
	}

	inline int col(const int rowIndx) {
		return cols[rowIndx];
	}

	ImageLane(int nrows, Parabola p)
	{
		parabola = p;
		confidence = p.totalFitness;
		cols = new int[nrows];
		minRow = p.minDomain;
		maxRow = p.maxDomain;
		for(int r = minRow; r <=maxRow; r++)
			cols[r] = (int) p.eval((float) r);
	}

	void overlayImage(Mat_<Vec3b> &img, Vec3b color, int width)
	{
		int halfWidth = width / 2;		
		for(int r = minRow; r <= maxRow; r++)
		{
			int predCol = cols[r];			
			for(int pixelToColor = predCol - halfWidth; pixelToColor <= predCol + halfWidth; pixelToColor++)
			{
				if(pixelToColor >= 0 && pixelToColor < img.cols)
				{
					img[r][pixelToColor] = color;					
				}
			}			
		}
	}

	~ImageLane() {
		delete [] cols;
	}

	int * cols;
	//domain = [minrow, maxRow)
	int minRow;
	int maxRow;
	Color color;
	double confidence;
	Parabola parabola;
};

#endif