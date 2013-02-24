#ifndef PARABOLA_H
#define PARABOLA_H

#define DEGREE 2

#include "cv.h"
#include <stdio.h>
#include <cmath>

class Parabola
{
public:
	//Domain bounds [minDomain, maxDomain)
	int minDomain;
	int maxDomain;
	//Coeffs
	double coeffs[DEGREE];
	double totalFitness;

	inline float eval(float row)
	{
		// Horner's method for evaluating polynomial
		double cumsum = 0;

		// coeffs[0] is for largest power of row
		for(int i = 0; i < DEGREE; i++) {
			cumsum *= row;
			cumsum += coeffs[i];
		}

		return (float) cumsum;
	}
};

//Gives the parabola determined by 3 points. Returns a parabola with all parameters set to 0 if underdetermined
inline Parabola parabolaForPoints(float* x, float* y)
{
	CvMat * vandermonde = cvCreateMat(DEGREE, DEGREE, CV_32FC1);		
	CvMat * yMat = cvCreateMat(DEGREE, 1, CV_32FC1);		
	for(int i = 0; i < DEGREE; i++) 
	{
		for(int j = 0; j < DEGREE; j++) 
			cvmSet(vandermonde, i, j, pow(x[i], (float)(DEGREE - 1.0 - j)));
		cvmSet(yMat, i, 0, y[i]);
	}
	
	CvMat * inverse = cvCreateMat(DEGREE, DEGREE, CV_32FC1);
	cvInvert(vandermonde, inverse, CV_LU);	
	CvMat * coeffs = cvCreateMat(DEGREE, 1, CV_32FC1);
	cvMatMul(inverse, yMat, coeffs);
	Parabola retval;
	for(int i = 0; i < DEGREE; i++)
		retval.coeffs[i] = cvmGet(coeffs, i, 0);	
	cvReleaseMat(&vandermonde);
	cvReleaseMat(&inverse);
	cvReleaseMat(&coeffs);
	cvReleaseMat(&yMat);
	return retval;
}

#endif