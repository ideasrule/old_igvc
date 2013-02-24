#ifndef PARABOLA_FITTER_H
#define PARABOLA_FITTER_H

#include "gendefs.h"
#include "Parabola.h"
#include "ImageLane.h"
#include "Frame.h"
#include <vector>
#include "cv.h"
using namespace std;
using namespace cv;
using namespace Pave_Libraries_Camera;

class ParabolaFitter {

public:
	ParabolaFitter(int nrows, int ncols);
	~ParabolaFitter();
	void findFit(Mat_<float> &img, int nIterations, int nParabolas, vector<ImageLane *>& lanes);	
private:
	float * cumsum;
	void sampleFromCumsum(float &row, float &col, int minRow, int maxRow);
	//Finds the best parabola in the range minRow to maxRow, subject to the constraint that it pass through fixedpoints
	Parabola bestParabola(Mat_<float> &img, int nIterations, int minRow, int maxRow, int nFixedPoints, int * fixedRows, int * fixedCols);	
	void findLaneExtent(Parabola& p, Mat_<float> &img);
	void extendLaneFit(ImageLane * lane, Mat_<float> &img);
	int nrows;
	int ncols;
};

#endif