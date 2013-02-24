#include "GroundPolyFitter.h"
#include "PointStructs.h"
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include "highgui.h"

using Pave_Libraries_Geometry::point2d;

// These values were computed by the separate solution HomographyCalcuation.
// Points on the ground were compared with points in the camera image and
//   cv::findHomography was used to determine the values below.
// Valid only for 640x480 images.
//static double homographyValues1D[] = {
//	-5.0202103, 0.35807403, 1477.5916,
//	0.034743867, 2.3800171, -2533.1646,
//	-0.0010277406, -0.029034168, 1
//};

static double homographyInvValues1D[] = {
	-0.1975,   -0.1201,  -12.2772,
    0.0071,   -0.0097,  -35.1509,
    0.0000,   -0.0004,   -0.0332
};

//const cv::Mat_<double> GroundPolyFitter::homography(3, 3, homographyValues1D);
const cv::Mat_<double> GroundPolyFitter::homographyInv(3, 3, homographyInvValues1D);
//const cv::Mat_<double> GroundPolyFitter::homography(cv::Mat_<double>::eye(3, 3));
const int GroundPolyFitter::RANSAC_MIN_SAMPLE = 3;
const double GroundPolyFitter::RANSAC_SQ_DISTANCE_THRESHOLD = 150.0;
const int GroundPolyFitter::RANSAC_NUM_ITERATIONS = 150;
const double GroundPolyFitter::RANSAC_CONFIDENCE_HIGH = 400.0;
const double GroundPolyFitter::RANSAC_CONFIDENCE_LOW = 250.0;
const double GroundPolyFitter::RANSAC_ACCEPT_PERCENT = 0.75;


//template<int degree>
GroundPolyFitter::GroundPolyFitter(int nrows, int ncols) :
	nrows(nrows), ncols(ncols)
{

	double groundPointInit[] = { 0.0, 0.0, 1.0 }; // homogeneous coordinates
	cv::Mat_<double> groundPoint(3, 1, groundPointInit);
	cv::Mat_<double> pixelPoint(3, 1);

	// create the 2d array groundToPixel
	groundToPixel = new RowCol*[nrows];
	RowCol *groundToPixelPtr1 = new RowCol[nrows*ncols]();
	groundToPixelPtr = groundToPixelPtr1;
	for( int i = 0; i < nrows; i++) { 
		*(groundToPixel + i) = groundToPixelPtr1; 
		groundToPixelPtr1 += ncols; 
	}

	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			groundPoint(0, 0) = (c - 400) * 2;
			groundPoint(1, 0) = (r - 480) * (-2);

			pixelPoint = homographyInv * groundPoint;

			groundToPixel[r][c].row = (int)(pixelPoint(1, 0)/pixelPoint(2, 0));
			groundToPixel[r][c].col = (int)(pixelPoint(0, 0)/pixelPoint(2, 0));
		}
	}

	// temporary variables for passing data to fitting algorithms
	// nrows * ncols is the maximum number of points that could be fit
	rowColX = new double[nrows * ncols];
	rowColY = new double[nrows * ncols];
}

//template<int degree>
GroundPolyFitter::~GroundPolyFitter(void)
{
	delete [] rowColX;
	delete [] rowColY;
	delete [] groundToPixel;
	delete [] groundToPixelPtr;
}

//template<int degree>
void GroundPolyFitter::pairVectorToRotatedArrays(
				const std::vector<RowCol>& v,
				double x[], double y[],
				double& theta)
{
	// Do Principle component analysis to find the most likely direction of directrix
	cv::Mat pcadata((int)v.size(), 2, CV_64F);
	for (int i = 0; i < (int)v.size(); ++i) {
		pcadata.at<double>(i, 0) = v[i].col;
		pcadata.at<double>(i, 1) = v[i].row;
	}

	cv::PCA pca(pcadata, cv::Mat(), CV_PCA_DATA_AS_ROW, 1);
	theta = atan2(pca.eigenvectors.at<double>(0, 1), pca.eigenvectors.at<double>(0, 0));
	double ct = cos(theta), st = sin(theta);
	
	// do passive rotation of coordinates (by -theta)to direction of principal e-vector
	for (int i = 0; i < (int)v.size(); ++i) {
		double tempX = v[i].col, tempY = v[i].row;
		rowColX[i] =  ct * tempX + st * tempY;
		rowColY[i] = -st * tempX + ct * tempY;
	}
}

// returns number of polynomials successfully matched. 
// see definition of constants at top of file for definite of "success"
//template<int degree>
int GroundPolyFitter::findLanes(const cv::Mat_<float>& img,
								vector<Polynomial<degree>>& bestPolynomial,
								vector<GroundPolyLane<degree>>& gpl,
								vector<double>& bestError,
								int numPoly)
{
	cv::Size size = img.size();
	if(nrows != size.height || ncols != size.width)
		return false;
	bestError.clear();
	bestPolynomial.clear();
	gpl.clear();

	// use RANSAC to fit a polynomial to the ground points

	vector<RowCol> whitePixels;   // all white pixels in the image, calculated once
	vector<RowCol> inliers;      // the white pixels that are inliers in the current iteration
	vector<int> bestConsensusIndex;  //index into whitePixels of the set of inliers with the highest score
	vector<int> currConsensusIndex;

#ifdef SHOW_VISUALIZATIONS
	cv::Mat rectifiedImage(nrows, ncols, CV_8U, cv::Scalar(0));
	cv::Mat rectifiedImage2(nrows, ncols, CV_8U, cv::Scalar(0));
#endif

	for (int r = 0; r < nrows; r++) {
		for (int c = 0; c < ncols; c++) {
			int row = groundToPixel[r][c].row;
			int col = groundToPixel[r][c].col;
			if (row  > 100 && row < nrows && col > 0 && col < ncols &&
				img(row, col) > 0) {
#ifdef SHOW_VISUALIZATIONS
				rectifiedImage.at<unsigned char>(r, c) = 255;
#endif
				whitePixels.push_back((RowCol(r, c)));
			}
		}
	}

	bestError.resize(numPoly);
	bestPolynomial.resize(numPoly);

	int polyIdx;
	for (polyIdx = 0; polyIdx < numPoly; ++polyIdx) {

		if(whitePixels.size() < 50)
			break;

		//// print the image with one line removed
		//if (polyIdx == 1) {
		//	for (int i = 0; i < (int)whitePixels.size(); ++i) 
		//		rectifiedImage2.at<unsigned char>(whitePixels[i].row, whitePixels[i].col) = 255;
		//}
		//cv::imshow("One line removed", rectifiedImage2);

		bestError[polyIdx] = 0.0;
		bestConsensusIndex.resize(0);

		for(int iterations = 0; iterations < RANSAC_NUM_ITERATIONS; iterations++) {
			inliers.resize(0);
			currConsensusIndex.resize(0);

			// first, take an initial random sample to be the first inliers
			
			//vector<int> indexList;
			for(int i = 0; i < RANSAC_MIN_SAMPLE; i++) {
				int index = (int) (rand() / (double) RAND_MAX * ((double) whitePixels.size() - 1));
				//indexList.push_back(index);
				// make sure indices don't repeat
				bool cont = false;
				for (int j = 0; j < i; ++j) {
					if (index == currConsensusIndex[j]) {
						i--;
						cont = true;
					}
				}
				if (cont) continue;


				currConsensusIndex.push_back(index);
				inliers.push_back(whitePixels[index]);
			}

	#if SHOW_VISUALIZATIONS 
			originalInliers = inliers; // save, for debugging
	#endif

			// find a model for the inliers
			double theta;
			pairVectorToRotatedArrays(inliers, rowColX, rowColY, theta);

			Polynomial<degree> inlierPoly = Polynomial<degree>::fitRegressionPoly(
					rowColX, rowColY, RANSAC_MIN_SAMPLE);
			inlierPoly.setTheta(theta);

			// check the model with unused points to see if those are inliers too
			// the points left in whitePixels are not yet in the model
			int j = 0;
			int oldConsensusSize = (int)currConsensusIndex.size();
			std::sort(currConsensusIndex.begin(), currConsensusIndex.end()); //this is ok since vector is still small
			for(int i = 0; i < (int)whitePixels.size(); i++) {
				// make sure these aren't the original inliers
				if (j < oldConsensusSize && i == currConsensusIndex[j]) {
					j++;
					continue;
				}
				if(inlierPoly.rotatedParabolaSqDistance(whitePixels[i].col, whitePixels[i].row) < RANSAC_SQ_DISTANCE_THRESHOLD) {
					inliers.push_back(whitePixels[i]);
					currConsensusIndex.push_back(i);
				}
			}
	#if SHOW_VISUALIZATIONS 
			extendedInliers = inliers; // save, for debugging
	#endif

			// fit new model with larger inlier set and compare to previous best
			pairVectorToRotatedArrays(inliers, rowColX, rowColY, theta);

			Polynomial<degree> newInlierPoly = Polynomial<degree>::fitRegressionPoly(
				rowColX, rowColY, (int)inliers.size());
			newInlierPoly.setTheta(theta);

			double sumError = 0.0;
			for(int i = 0; i < (int)inliers.size(); i++) 
				sumError += 1 / (1 + newInlierPoly.rotatedParabolaSqDistance(inliers[i].col, inliers[i].row));
			

			if(sumError > bestError[polyIdx]) {
				bestError[polyIdx] = sumError;
				bestConsensusIndex = currConsensusIndex;
				bestPolynomial[polyIdx] = newInlierPoly;
			}

			//cout << "Inliers size: " << extendedInliers.size() << endl;

	#if SHOW_VISUALIZATIONS & 0
			cv::Mat_<cv::Vec3b> laneImage(img.size());
			
			// copy the image to color so we can overlay points and lines later
			for(int r = 0; r < laneImage.size().height; r++) {
				for(int c = 0; c < laneImage.size().width; c++) {
					cv::Vec3b::value_type u = rectifiedImage.at<unsigned char>(r, c);
					laneImage(r, c) = cv::Vec3b(u, u, u);
				}
			}
			
			// draw the extended set of inlier points
			for(int i = 0; i < extendedInliers.size(); i++) {
				RowCol& rc = extendedInliers[i];
				cv::circle(laneImage, cv::Point2d(rc.col, rc.row), 1, cv::Scalar(cv::Vec3b(0, 0, 255)), 4);
			}

			// draw the fitted parabola on the image
			for(int r = 0; r < laneImage.size().height; r++) {
				for(int c = 0; c < laneImage.size().width; c++) {
					if(newInlierPoly.rotatedParabolaSqDistance(c, r) < 5)
						laneImage(r, c) = cv::Vec3b(255, 0, 0);
				}
			}

			// draw the original inlier points
			for(int i = 0; i < originalInliers.size(); i++) {
				RowCol& rc = originalInliers[i];
				cv::circle(laneImage, cv::Point2d(rc.col, rc.row), 2, cv::Scalar(cv::Vec3b(0, 255, 0)), 4);
			}

			std::cout << iterations << "\t" << sumError << std::endl;
			cv::imshow("Lane Image", laneImage);

			cv::waitKey();
	#endif
		}

		// check to see if we are confident enough about this line
		if (bestError[polyIdx] < RANSAC_CONFIDENCE_LOW ||
			(bestError[polyIdx] >= RANSAC_CONFIDENCE_LOW &&
			 bestError[polyIdx] < RANSAC_CONFIDENCE_HIGH &&
			 (double)bestConsensusIndex.size() / (double)whitePixels.size() < RANSAC_ACCEPT_PERCENT))
			 break;

/*		vector<RowCol> pointsToSort(bestConsensusIndex.size());
		for(int ip = 0; ip < bestConsensusIndex.size(); ip++) {
			pointsToSort[ip] = whitePixels[bestConsensusIndex[i]];
		}
		std::sort(pointsToSort.begin(), pointsToSort.end());

		// find min and max x-values to limit the domain
		RowCol* rc = &whitePixels[bestConsensusIndex[i]];
		std::sort(rc, rc + N);
		p.setMinX(x[(int) (0.1 * N)]);
		p.setMaxX(x[N - 1 - (int) (0.1 * N)]); // handles small (degenerate) N
*/
		// remove the found points from whitePixels and search again
		// overwrites elements indexed by bestConsensusList with end of whitePixel
		std::sort(bestConsensusIndex.begin(), bestConsensusIndex.end());
		int j = (int)whitePixels.size() - 1;
		for (int i = (int)bestConsensusIndex.size() - 1; i >= 0 && j >= 0; --i, --j) {
			whitePixels[bestConsensusIndex[i]] = whitePixels[j];
		}
		whitePixels.resize(whitePixels.size()-bestConsensusIndex.size());
	}


#if SHOW_VISUALIZATIONS // display final result
	cv::Mat_<cv::Vec3b> laneImage(rectifiedImage.size());

	// copy the image to color so we can overlay points and lines later
	for(int r = 0; r < laneImage.size().height; r++) {
		for(int c = 0; c < laneImage.size().width; c++) {
			cv::Vec3b::value_type u = rectifiedImage.at<unsigned char>(r, c);
			laneImage(r, c) = cv::Vec3b(u, u, u);
		}
	}
	
	// draw the extended set of inlier points
	//for(int i = 0; i < (int)bestConsensusSet.size(); i++) {
	//	RowCol& rc = bestConsensusSet[i];
	//	cv::circle(laneImage, cv::Point2d(rc.col, rc.row), 2, cv::Scalar(cv::Vec3b(0, 0, 255)), 1);
	//}
	//cv::imshow("Lane Image before fit", laneImage);
	//cv::waitKey(0);

	cout << polyIdx << " lanes detected. " << endl;
	for (int i = 0; i < polyIdx; ++i) {
		// draw the fitted parabola on the image
		for(int r = 0; r < laneImage.size().height; r++) {
			for(int c = 0; c < laneImage.size().width; c++) {
				double rotatedX = (double)c;
				double rotatedY = (double)r;
				bestPolynomial[i].rotatePointToParabolaCoordinates(rotatedX, rotatedY);
				if (rotatedX < bestPolynomial[i].getMinX() || rotatedX > bestPolynomial[i].getMaxX())
					continue;
				if(bestPolynomial[i].sqDistance(rotatedX, rotatedY) < 5)
					laneImage(r, c) = cv::Vec3b(255, 0, 0);
			}
		}

		// draw the original inlier points
	/*		for(int i = 0; i < gpf.originalInliers.size(); i++) {
			RowCol& rc = gpf.originalInliers[i];
			cv::circle(laneImage, cv::Point2d(rc.col, rc.row), 2, cv::Scalar(Vec3b(0, 255, 0)), 4);
		}
	*/
		cout << "Best error of Lane " << i+1 << ": " << bestError[i] << endl;
	}
	cv::imshow("Lane Image", laneImage);
	cv::waitKey(0);

#endif

/*	// testing code
	double bp1x[] = { 0.0, 320.0, 640.0 };
	double bp1y[] = { 320.0, 0.0, 320.0 };
	Polynomial<2> bp1 = Polynomial<2>::findPoly(bp1x, bp1y);
	bestPolynomial.clear();
	bestPolynomial.push_back(bp1);
*/
	// return 3 points per lane that completely determine each parabola
	double polyPointsX[3], polyPointsY[3];
	gpl.reserve(3);
	for(int pn = 0; pn < polyIdx; pn++) {
		// choose the x-values of the points to get
		// send the endpoints and midpoint (on the x-axis)
		polyPointsX[0] = bestPolynomial[pn].getMinX();
		polyPointsX[2] = bestPolynomial[pn].getMaxX();
		polyPointsX[1] = (polyPointsX[0] + polyPointsX[2]) / 2.0;

		for(int i = 0; i < 3; i++) {
			// find the unrotated x and y values
			double x, y;
			bestPolynomial[pn].evalRotated(polyPointsX[i], x, y);

			// translate and scale to correct ground coordinates
			polyPointsX[i] = (x - 400) * 2.0;
			polyPointsY[i] = (y - 480) * -2.0;
		}

		// the points sent to costmapgenerator are in real ground coordinates
		// in cm with origin at the robot. the theta's are the angle of the
		// principle component, so costmapgen doesn't have to recompute it.
		// be careful that in costmapgen you have to negate theta, since
		// the y-axis is flipped.
		gpl.push_back(GroundPolyLane<degree>(polyPointsX, polyPointsY, bestPolynomial[pn].getTheta()));
	}

	return polyIdx;
}