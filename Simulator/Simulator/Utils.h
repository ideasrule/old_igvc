#pragma once

#include <math.h>
#include <stdlib.h>


class Utils
{
public:
	static double Uniform();
	static double Exponential(double lambda);

	static double Gaussian();

	//Random integer between [0,max]
	static int RandInt(int max);

	//Puts angle in [0,2pi]
	static double SquashAngle(double angle);
};
