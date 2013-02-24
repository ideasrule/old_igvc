#ifndef GROUND_POLY_FITTER_H
#define GROUND_POLY_FITTER_H

#include "cv.h"
#include "Point2D.h"
#include "Polynomial.h"
#include <vector>
#include "RowCol.h"
#include "GroundPolyLane.h"

using Pave_Libraries_Geometry::Point2D;

const int degree = 2;

#define SHOW_VISUALIZATIONS 1

class GroundPolyFitter
{
public:
	//static const cv::Mat_<double> homography;
	static const cv::Mat_<double> homographyInv;

	GroundPolyFitter(int nrows, int ncols);
	~GroundPolyFitter(void);

	int findLanes(const cv::Mat_<float> &img,
				  vector<Polynomial<degree>>& bestPolynomial,
				  vector<GroundPolyLane<degree>>& gpl,
				  vector<double>& bestError, int numPoly = 1);

#if SHOW_VISUALIZATIONS
	// visualization information
	std::vector<RowCol> originalInliers;  // the original randomly chosen inliers
	std::vector<RowCol> extendedInliers;  // the expanded set of inliers
#endif

private:

	RowCol **groundToPixel;
	RowCol *groundToPixelPtr;

	double *rowColX;
	double *rowColY;

	int nrows, ncols;

	void pairVectorToRotatedArrays(
				const std::vector<RowCol>& v,
				double x[], double y[],
				double& theta);

	static const int RANSAC_MIN_SAMPLE;
	static const double RANSAC_SQ_DISTANCE_THRESHOLD;
	static const int RANSAC_NUM_ITERATIONS;     

	// anything with score above HIGH is automatically accepted,
	// those with score between HIGH and LOW is checked to see whether
	// the [number of lane pixels]/[number of remaining white pixels] > RANSAC_ACCEPT_PERCENT
	static const double RANSAC_CONFIDENCE_HIGH;
	static const double RANSAC_CONFIDENCE_LOW;
	static const double RANSAC_ACCEPT_PERCENT;
};

#endif // GROUND_POLY_FITTER_H