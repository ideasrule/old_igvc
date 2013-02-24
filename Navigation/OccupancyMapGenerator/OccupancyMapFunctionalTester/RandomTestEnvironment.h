#pragma once

#include "../OccupancyMapGenerator/types.h"
#include "Array2D.h"
#include "Memory.h"

#include "cv.h"
#include "boost/random.hpp"

namespace Pave_Libraries_Navigation {

// Random environment with obstacles for robot testing
class RandomTestEnvironment
{
public:
    const int X_MAX;
    const int Y_MAX;
    const int N_OBSTACLE;
	const double MAX_OBSTACLE_RADIUS;
    const int NUM_PATH_POINTS;

    enum CellState {
		OBSTACLE,
		PATH,
		FREE,
		FOV // should never be assigned in the array because it's defined based on FOV parameters
	};

    RandomTestEnvironment(int x, int y, int nObs, double maxObsRadius, int numPathPoints);
    ~RandomTestEnvironment(void);

    static const int IMAGE_WIDTH;
    static const int IMAGE_HEIGHT;
    cv::Mat_<cv::Vec3b> copyToImage() const;
    cv::Mat_<cv::Vec3b> copyToImageWithFov(int segment, int index, Length range, Angle fov) const;

	std::vector< std::vector<GridSquare> > getPath() const { return path; }

	struct Measurement {
		Length distance;
		Angle angle;  // > 0 is to the right, < 0 to the left

		Measurement(Length d, Angle a) : distance(d), angle(a) {}
	};

	// gets the measurements corresponding to the field of view from getPath()[i]
	// range and fov specify the field of view (fov/2 radians on each side of the robot)
	// dfov specifies the angular precision of the measurements
	//   the function returns approximately fov/dfov measurements
	shared_ptr< std::vector<Measurement> > getMeasurements(int segment, int index, Length range, Angle fov, Angle dfov);

    // gets a simulated state: x, y coordinates and heading
    void getState(double& x, double& y, double& heading, int segment, int index);

private:
    Array2D<CellState> env;
	std::vector< std::vector<GridSquare> > path; // piecewise, each vector is a segment

    // for randomness
    typedef boost::variate_generator<boost::mt19937&, boost::uniform_int<> > RandomInt;
    typedef boost::variate_generator<boost::mt19937&, boost::uniform_real<> > RandomReal;
    boost::mt19937 gen;
    RandomInt randomX;
    RandomInt randomY;
	RandomReal randomObstacleRadius;

    int numSquares() const { return X_MAX * Y_MAX; }
    int numObstacles() const { return N_OBSTACLE; }
    int numPathPoints() const { return NUM_PATH_POINTS; }

    typedef cv::Scalar_<uchar> PixelColor;
    static PixelColor getColor(CellState gs);

    GridSquare randomFreeGridSquare();
	double distance(GridSquare gs1, GridSquare gs2) const;
	shared_ptr< std::vector<GridSquare> > randomObstacle();

	Angle findRobotAngle(int segment, int index) const;
	Angle findRobotAngleTo(int segment, int index, GridSquare gs) const;
	static Angle angleDifference(Angle a1, Angle a2);
	shared_ptr< std::vector<GridSquare> > getFovSquares(int segment, int index, Length range, Angle fov) const;

    CellState getState(GridSquare gs) const { return env.cell(gs.x, gs.y); }
    void setState(GridSquare gs, CellState cs) { env.cell(gs.x, gs.y) = cs; }

    void drawLinePath(GridSquare start, GridSquare end, std::vector<GridSquare>& path);
};

}
