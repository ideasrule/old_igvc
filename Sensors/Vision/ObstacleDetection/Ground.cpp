#include "Ground.h"

#include <vector>
#include <cstdlib>
#include <cmath>

#include <iostream> // for debugging spew
#include <iomanip>

using std::vector;

//const float MIN_OBSTACLE_HEIGHT = -0.9f;
const float MIN_OBSTACLE_HEIGHT = 0.28f;

const int GROUND_RANSAC_NUM_ITERATIONS = 50;
const double GROUND_RANSAC_MIN_INLIER_DISTANCE = 1.5; // meters from the robot
const double GROUND_RANSAC_MIN_Z_DOT = 0.7; // approximately 45 degree deviation from robot's current orientation
const double GROUND_RANSAC_INLIER_THRESHOLD = 0.08;
const double GROUND_OBSTACLE_MAX_DEPTH = 7.0; // meters

static double vecLength(Vec3D vec)
{
	return std::sqrt(vec.dot(vec));
}

// using vector to a point, unit normal vector to a plane, and a vector
//   to a point in the plane, find distance from point to plane
static double distancePointToPlane(const Vec3D& point,
								   const Vec3D& unitN,
								   const Vec3D& pointInPlane)
{
	Vec3D diff = point - pointInPlane;
	return unitN.dot(diff);
}

static double distancePointToPlane(const double point[],
								   const double unitN[],
								   const double pointInPlane[])
{
	double dotsum = 0.0;
	for(int i = 0; i < 3; i++) {
		double d = point[i] - pointInPlane[i];
		dotsum += d * unitN[i];
	}
	return dotsum;
}

Ground::Ground() {}

Ground::~Ground() {}

bool Ground::process(shared_ptr<Frame> frm) {
	
	// Start by clearing obstacle matrix
	// this should be zero already
	frm->setObstacle((unsigned char)0);

    // Setup the overlay image
    frm->color.copyTo(overlay_);

	for (int iRow = 0; iRow < IMAGE_HEIGHT; iRow++) {
		for (int iCol = 0; iCol < IMAGE_WIDTH; iCol++) {
			cloud[iRow][iCol] = frm->transformedCloud[iRow][iCol];
		}
	}

	Vec3D bestVec3D;
	vector<RowCol> bestInlierRowCols;
	bool haveEstimate = false;//estimateGroundPlane(frm, bestVec3D, bestInlierRowCols, bestScore);

	int iRow, iCol;

	// coordinates of transformedCloud defined as
	// up = z, right = x; depth = y, 
	double pt_d[3], unitGroundN_d[3], pointOnGround_d[3];
	Vec3D pointOnGround = bestVec3D; // length of bestVec3D is perpendicular distance from origin to plane
	Vec3D unitGroundN = bestVec3D / vecLength(bestVec3D); // unit normal to ground

	if(haveEstimate) {
		for(int i = 0; i < bestInlierRowCols.size(); i++) {
			RowCol& rc = bestInlierRowCols[i];
			overlay_[rc.row][rc.col] = cv::Vec3b(0, 255, 0);
		}

		if(unitGroundN(2, 0) < 0) { // negative z-component, normal points down
			unitGroundN *= -1;
		}

		for(int i = 0; i < 3; i++) {
			unitGroundN_d[i] = unitGroundN(i, 0);
			pointOnGround_d[i] = pointOnGround(i, 0);
		}
	}

	for(iRow = 0; iRow < IMAGE_HEIGHT; iRow++) {
		for(iCol = 0; iCol < IMAGE_WIDTH; iCol++) {
			if (!frm->validArr[iRow][iCol]) continue;

			cv::Point3f& p = cloud[iRow][iCol];

			double height;
			if(haveEstimate) {
				pt_d[0] = p.x;
				pt_d[1] = p.y;
				pt_d[2] = p.z;
				height = distancePointToPlane(pt_d, unitGroundN_d, pointOnGround_d);
			}
			else {
				height = p.z;
			}

			if(height > MIN_OBSTACLE_HEIGHT && p.y < GROUND_OBSTACLE_MAX_DEPTH) {
				frm->obstacle[iRow][iCol] = UCHAR_SAT;
				overlay_[iRow][iCol] = cv::Vec3b(255,0,0);
			}
		}
	}

	if(haveEstimate) {
		for(iRow = 0; iRow < IMAGE_HEIGHT; iRow++) {
			for(iCol = 0; iCol < IMAGE_WIDTH; iCol++) {
				if (!frm->validArr[iRow][iCol]) continue;

				Vec3D pt;
				cloudToVec3D(iRow, iCol, pt);
				double height = distancePointToPlane(pt, unitGroundN, pointOnGround);

				if(height > MIN_OBSTACLE_HEIGHT) {
					overlay_[iRow][iCol] = cv::Vec3b(255,0,0);
				}
			}
		}
	}

	return true;

}

static int getRandInt(int exclusiveMax)
{
	int val = 0;
	do {
		val = (int) (((double) rand() / (double) RAND_MAX) * (exclusiveMax - 1.0));
	} while(val < 0 || val >= exclusiveMax);

	return val;
}

void Ground::cloudToVec3D(int r, int c, Vec3D& vec)
{
	cv::Point3f& p = cloud[r][c];

	vec = Vec3D(3, 1);
	vec(0, 0) = p.x;
	vec(1, 0) = p.y;
	vec(2, 0) = p.z;
}

void Ground::getGuessVector(Vec3D& guessVec)
{
	int guessRow = getRandInt(IMAGE_HEIGHT - 1);
	int guessCol = getRandInt(IMAGE_WIDTH - 1);

	cloudToVec3D(guessRow, guessCol, guessVec);
}

// the robot pitches and rolls as it drives on uneven terrain
// try to determine the ground plane using RANSAC
// variables of fit: A, B, C such that Ax + By + C = z
// where x and y are planar coordinates and z is height
bool Ground::estimateGroundPlane(
						Frame &frm,
						Vec3D& bestVec3D,
						vector<RowCol>& bestInlierRowCols,
						double& bestScore)
{
	Vec3D origin = Vec3D::zeros(3, 1);

	// vector from origin, perpendicular to plane, length is minimum distance
	bestVec3D = origin;

	int workingInliersNum;
	int *workingInliers = buf1; // index with IMAGE_WIDTH*row + col

	int bestInliersNum;
	int *bestInliers = buf2; // index with IMAGE_WIDTH*row + col

	bestScore = 0.0;

	for (int iterations = 0; iterations < GROUND_RANSAC_NUM_ITERATIONS; iterations++)
	{
		workingInliersNum = 0;

		Vec3D v1, v2, v3;
		getGuessVector(v1);
		getGuessVector(v2);
		getGuessVector(v3);

		Vec3D v21 = v2 - v1;
		Vec3D v31 = v3 - v1;

		// skip cases where points are too close to the robot
		if(vecLength(v1) < GROUND_RANSAC_MIN_INLIER_DISTANCE ||
		   vecLength(v2) < GROUND_RANSAC_MIN_INLIER_DISTANCE ||
		   vecLength(v3) < GROUND_RANSAC_MIN_INLIER_DISTANCE)
			continue;
	
		// the normal to the plane formed by v1 and v2
		Vec3D unitN = v21.cross(v31);
		// the unit normal
		unitN = unitN / vecLength(unitN);

		// if plane is unreasonably steep, don't consider it
		double zComponent = unitN(2, 0); // z component of normal estimates deviation from plane of robot
		if(zComponent < GROUND_RANSAC_MIN_Z_DOT)
			continue;

		// a unit vector in the plane
//		Vec3D unitP = v21 / vecLength(v21); // v21 arbitrarily chosen; can use v31

		// v1 arbitrary, can use v2, v3
		double distOriginToPlane = distancePointToPlane(origin, unitN, v1);
	
		// expand inlier set
/*		for(int r = 0; r < IMAGE_HEIGHT; r++) {
			for(int c = 0; c < IMAGE_WIDTH; c++) {
				Vec3D pt;
				cloudToVec3D(r*SF, c*SF, pt);
				double d = fabs(distancePointToPlane(pt, unitN, v21));

				if(d < GROUND_RANSAC_INLIER_THRESHOLD) {
					// point is an inlier
					workingInliers[workingInliersNum++] = r * IMAGE_WIDTH + c;
				}
			}
		}
*/
		// expand inliers
		int N = IMAGE_WIDTH * IMAGE_HEIGHT;
		int sampleCount = 0;
		Vec3D pt;
		double pt_d[3];
		double unitN_d[3];
		double v21_d[3];
		for(int i = 0; i < 3; i++) {
			unitN_d[i] = unitN(i, 0);
			v21_d[i] = v21(i, 0);
		}
		for(int i = 0; i < 500; ) {
			sampleCount = (sampleCount + getRandInt(N)) % N;
			int r = sampleCount / IMAGE_WIDTH;
			int c = sampleCount % IMAGE_WIDTH;
			cv::Point3f& p = cloud[r][c];
			pt_d[0] = p.x;
			pt_d[1] = p.y;
			pt_d[2] = p.z;
			double d = fabs(distancePointToPlane(pt_d, unitN_d, v21_d));

			if(d < GROUND_RANSAC_INLIER_THRESHOLD) {
				// point is an inlier
				workingInliers[workingInliersNum++] = sampleCount;
				i++;
			}
		}

		// fit a 3D plane to the inlier set using 3D planar least-squares regression
		// use http://www.geometrictools.com/Documentation/LeastSquaresFitting.pdf as of 2010-06-07 -- josh newman
		double sumSqX, sumSqY, sumXY, sumXZ, sumYZ, sumX, sumY, sumZ;
		sumSqX = sumSqY = sumXY = sumXZ = sumYZ = sumX = sumY = sumZ = 0.0;
		for(int i = 0; i < workingInliersNum; i++) {
			int r = workingInliers[i] / IMAGE_WIDTH;
			int c = workingInliers[i] % IMAGE_WIDTH;
			cv::Point3f& p = cloud[r][c];
			float& x = p.x;
			float& y = p.y;
			float& z = p.z;
			
			sumX += x; sumSqX += x * x;
			sumY += y; sumSqY += y * y;
			sumZ += z;
			sumXY += x * y;
			sumXZ += x * z;
			sumYZ += y * z;
		}
		double leastSquaresMatVals[] = 
			{ sumSqX, sumXY,  sumX,
			  sumXY,  sumSqY, sumY,
			  sumX,   sumY,   1.0 };
		cv::Mat_<double> leastSquaresM(3, 3, leastSquaresMatVals);
		double leastSquaresVecVals[] = { sumXZ, sumYZ, sumZ };
		cv::Mat_<double> leastSquaresV(3, 1, leastSquaresVecVals);
		cv::Mat_<double> leastSquaresSol(leastSquaresM.inv() * leastSquaresV);

		Vec3D normalVec = leastSquaresSol;
		normalVec(2, 0) = -1.0; // 0 = Ax + By + C - z
		unitN = normalVec / vecLength(normalVec);
		double pipVals[] = { 0.0, 0.0, leastSquaresSol(2, 0) }; // z(0, 0) = (0)x + (0)y + C = C
		Vec3D pip(3, 1, pipVals);
		distOriginToPlane = distancePointToPlane(origin, unitN, pip);

		// make sure model includes significant set of inliers
		// if not, discard without error for efficiency
		// TODO

		// score the plane hypothesis by calculating score (using all inliers)
		// metric: sum of 1/(1 + distance)
		double sumScore = 0.0;
		for(int i = 0; i < workingInliersNum; i++) {
			int r, c;
			r = workingInliers[i] / IMAGE_WIDTH;
			c = workingInliers[i] % IMAGE_WIDTH;
			Vec3D pt;
			cloudToVec3D(r, c, pt);
			double d = distancePointToPlane(pt, unitN, pip);

			sumScore += 1 / (1 + d * d);
		}

		if(sumScore > bestScore) {  // found a new best
			bestVec3D = unitN * distOriginToPlane;
			std::swap(bestInliers, workingInliers); // swap the double-buffered pointers
			std::swap(bestInliersNum, workingInliersNum); // switch the numbers
			bestScore = sumScore;
		}
	}

	if(bestInliersNum > 0) {
		// bestPointsNum indicates how many are filled
		bestInlierRowCols.resize(bestInliersNum);

		for(int i = 0; i < bestInliersNum; i++) {
			RowCol rc(bestInliers[i] / IMAGE_WIDTH, bestInliers[i] % IMAGE_WIDTH);
			bestInlierRowCols[i] = rc;
		}
		return true;
	}
	else {
		return false;
	}

}