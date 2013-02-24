#include "GroundPolyFitter.h"
#include "CumulativeImageSampler.h"

// These values were computed by the separate solution HomographyCalcuation.
// Points on the ground were compared with points in the camera image and
//   cv::findHomography was used to determine the values below.
// Valid only for 640x480 images.
static double homographyValues1D[] = {
	-5.0202103, 0.35807403, 1477.5916,
	0.034743867, 2.3800171, -2533.1646,
	-0.0010277406, -0.029034168, 1
};

template<int d> const cv::Mat_<double> GroundPolyFitter<d>::homography(3, 3, homographyValues1D);
template<int d> const int GroundPolyFitter<d>::RANSAC_MIN_SAMPLE = 4;
template<int d> const double GroundPolyFitter<d>::RANSAC_SQ_DISTANCE_THRESHOLD = 3.0;
template<int d> const int GroundPolyFitter<d>::RANSAC_NUM_ITERATIONS = 10;

template<int degree>
GroundPolyFitter<degree>::GroundPolyFitter(int nrows, int ncols) :
	nrows(nrows), ncols(ncols), pixelToGround(nrows, ncols), allPixels(nrows * ncols)
{
	double pixelPointInit[] = { 0.0, 0.0, 1.0 }; // homogeneous coordinates
	cv::Mat_<double> pixelPoint(3, 1, pixelPointInit);
	cv::Mat_<double> groundPoint(3, 1);

	int i = 0;
	for(int r = 0; r < nrows; r++) {
		for(int c = 0; c < ncols; c++) {
			pixelPoint(1, 1) = (double) r;
			pixelPoint(1, 2) = (double) c;

			groundPoint = homography * pixelPoint;

			double x = groundPoint(1, 1);
			double y = groundPoint(1, 2);
			double scale = groundPoint(1, 3);
			pixelToGround(r, c).setX(x /scale);
			pixelToGround(r, c).setY(y /scale);

			allPixels[i] = RowCol(r, c);
			i++;
		}
	}
}

template<int degree>
GroundPolyFitter<degree>::GroundPolyFitter(void) :
	nrows(480), ncols(640), pixelToGround(nrows, ncols), allPixels(nrows * ncols)
{
	double pixelPointInit[] = { 0.0, 0.0, 1.0 }; // homogeneous coordinates
	cv::Mat_<double> pixelPoint(3, 1, pixelPointInit);
	cv::Mat_<double> groundPoint(3, 1);

	int i = 0;
	for(int r = 0; r < nrows; r++) {
		for(int c = 0; c < ncols; c++) {
			pixelPoint(1, 1) = (double) r;
			pixelPoint(1, 2) = (double) c;

			groundPoint = homography * pixelPoint;

			double x = groundPoint(1, 1);
			double y = groundPoint(1, 2);
			double scale = groundPoint(1, 3);
			pixelToGround(r, c).setX(x /scale);
			pixelToGround(r, c).setY(y /scale);

			allPixels[i] = RowCol(r, c);
			i++;
		}
	}
}

template<int degree>
GroundPolyFitter<degree>::~GroundPolyFitter(void)
{
}

template<int degree>
static void GroundPolyFitter<degree>::pairVectorToArrays(
				const std::vector<Point2D>& v,
				double x[], double y[])
{
	std::vector<Point2D>::iterator it;
	for(it = v.begin(); it < v.end(); it++) {
		x[i] = it->getX();
		y[i] = it->getY();
	}
}

template<int degree>
Point2D GroundPolyFitter<degree>::convertPixelToGround(const RowCol& rc)
{
	return pixelToGround(rc.row, rc.col);
}

// returns true if successful, false if error
template<int degree>
bool GroundPolyFitter<degree>::findLanes(const cv::Mat_<float> &img, Polynomial<degree>& retp)
{
	cv::Size size = img.size();
	if(nrows != size.width || ncols != size.height)
		return false;

	// use RANSAC to fit a polynomial to the ground points

	std::vector<Point2D> bestConsensusSet;
	double bestNormalizedSqError;
	Polynomial bestPolynomial;

	// temporary variables for passing data to fitting algorithms
	// nrows * ncols is the maximum number of points that could be fit
	double rowColX[nrows * ncols];
	double rowColY[nrows * ncols];

	CumulativeImageSampler sampler(img);
	std::vector<Point2D> inliers;

	for(int iterations = 0; iterations < RANSAC_NUM_ITERATIONS; iterations++) {

		// first, take an initial random sample to be the first inliers
		RowCol rcs[RANSAC_MIN_SAMPLE];
		sampler.multiSample(rcs, RANSAC_MIN_SAMPLE);
		for(int i = 0; i < RANSAC_MIN_SAMPLE; i++) {
			inliers.push_back(convertPixelToGround(rcs[i]));
		}

		// find a model for the inliers
		rowColVectorToArrays(inliers, rowColX, rowColY);
		Polynomial<degree> inlierPoly = Polynomial<degree>::fitRegressionPoly(
				rowColX, rowColY, RANSAC_MIN_SAMPLE);

		// check the model with unused points to see if those are inliers too
		std::vector<Point2D> remainingPoints;
		std::set_difference(allPixels.begin(), allPixels.end(),
							inliers.begin(), inliers.end(),
							remainingPoints.begin());
		
		std::vector<Point2D>::iterator it;
		for(it = remainingPoints.begin(); it < remainingPoints.end(); it++) {
			if(inlierPoly.sqDistance(it->getX(), it->getY()) < RANSAC_SQ_DISTANCE_THRESHOLD)
				inliers.push_back(*it);
		}

		// fit new model with larger inlier set and compare to previous best
		rowColVectorToArrays(inliers, rowColX, rowColY);
		Polynomial<degree> newInlierPoly = Polynomial<degree>::fitRegressionPoly(
				rowColX, rowColY, inliers.size());

		double sumSqError = 0.0;
		for(it = inliers.begin(); it < inliers.end(); it++) {
			sumSqError += newInlierPoly.sqDistance(it->getX(), it->getY());
		}
		double normalizedSqError = sumSqError / inliers.size();

		if(normalizedSqError < bestNormalizedSqError) {
			bestNormalizedSqError = normalizedSqError;
			bestConsensusSet = inliers;
			bestPolynomial = newInlierPoly;
		}

	}
}