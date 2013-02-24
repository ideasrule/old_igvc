#include "Utils.h"
#define PI 3.1415926535
double Utils::Uniform()
{
	return 1.0-(double)(rand()+1)/(double)(RAND_MAX+1);
}
//Returns a sample from an exponential random 
//variate with parameter lambda
double Utils::Exponential(double lambda)
{
	return -log(1.0-Uniform())/lambda;
}

//Calculates a gaussian random variable using the Box-Muller transform.
//Pretty inefficient, doesn't store away the other generated var
//From p61, Scheinerman, Edward. "C++ for Mathematicians: An Introduction for Students and Professionals" Chapman and Hall/CRC, 2006.
double Utils::Gaussian()
{
	double x,y,r;
 
	do {
		x = Uniform()*2.0-1.0; //uniform(-1,1)
		y = Uniform()*2.0-1.0;
		r = x*x + y*y;
	} while (r >= 1.0);
	
	double mu = sqrt(-2.0 * log(r) / r);

	return mu*x;
}


int Utils::RandInt(int max)
{
	return rand() % (max+1);
}

double Utils::SquashAngle(double angle) 
{
	return fmod(angle, 2*PI) + (angle < 0?2*PI:0);
}