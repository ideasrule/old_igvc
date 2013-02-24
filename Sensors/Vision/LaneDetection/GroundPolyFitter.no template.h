#ifndef GROUND_POLY_FITTER_H
#define GROUND_POLY_FITTER_H

#include "cv.h"
#include "Point2D.h"
#include "Polynomial.h"
#include <set>
#include "RowCol.h"

using Pave_Libraries_Geometry::Point2D;

template<int degree>
class GroundPolyFitter
{
public:
	static const cv::Mat_<double> homography;

	GroundPolyFitter(int nrows, int ncols);
	~GroundPolyFitter(void);

	bool findLanes(const cv::Mat_<float> &img, Polynomial<degree>& p);

private:
	cv::Mat_<Point2D> pixelToGround;
	int nrows, ncols;
	std::vector<RowCol> allPixels;

	static void pairVectorToArrays(
				const std::vector<Point2D>& v,
				double x[], double y[]);
	
	Point2D convertPixelToGround(const RowCol& rc);

	static const int RANSAC_MIN_SAMPLE;
	static const double RANSAC_SQ_DISTANCE_THRESHOLD;
	static const int RANSAC_NUM_ITERATIONS;
};

#endif // GROUND_POLY_FITTER_H