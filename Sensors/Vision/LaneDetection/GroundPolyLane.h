#pragma once

template <int degree>
class GroundPolyLane
{
public:
	double pointsX[degree + 1];
	double pointsY[degree + 1];
	double theta;

	// the points are in unrotated coordinates
	// to get correct points, rotate the points by theta
	GroundPolyLane(const double ptsX[], const double ptsY[], double theta) :
		theta(theta)
	{
		for(int i = 0; i < degree + 1; i++) {
			pointsX[i] = ptsX[i];
			pointsY[i] = ptsY[i];
		}
	}

	GroundPolyLane() : pointsX(0.0), pointsY(0.0), theta(0.0) {}
};
