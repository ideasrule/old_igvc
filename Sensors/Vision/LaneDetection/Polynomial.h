#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H

#include "newmatio.h"
#include "newmat.h"
#include <cmath>
#include <algorithm>

namespace nr {
	extern "C" {
		void fpoly(double x, double p[], int np);

		void lfit(double x[], double y[], double sig[], int ndat, double a[], int ia[],
		int ma, double **covar, void (*funcs)(double, double [], int));

		int *ivector(long nl, long nh);
		double *vector(long nl, long nh);
		double **matrix(long nrl, long nrh, long ncl, long nch);

		void free_ivector(int *v, long nl, long nh);
		void free_vector(double *v, long nl, long nh);
		void free_matrix(double **m, long nrl, long nrh, long ncl, long nch);
	}
}

template<int degree>
class Polynomial
{
public:
	// coefficients
	// coeffs[0] is for highest order term in x
	double coeffs[degree + 1];
	void setTheta(double theta) 
	{
		this->theta = theta;
		ct = cos(theta);
		st = sin(theta);
	}
	double getTheta(void) { return theta; }
	
	Polynomial() : theta(0.0), ct(1.0), st(0.0), minX(0.0), maxX(0.0) {}

	void setMinX(double minX) { this->minX = minX; }
	void setMaxX(double maxX) { this->maxX = maxX; }

	double getMinX(void) { return minX; }
	double getMaxX(void) { return maxX; }


	double eval(double x)
	{
		// Horner's method for evaluating polynomial
		double cumsum = 0;

		// coeffs[0] is for largest power of x
		for(int i = 0; i <= degree; i++) {
			cumsum *= x;
			cumsum += coeffs[i];
		}

		return cumsum;
	}

	// x is coordinate in polynomial coordinates, returns value in poly coords,
	// xRet and yRet has coord in regular coordinatesw
	double evalRotated(double x, double& xRet, double &yRet)
	{
		double yPoly = eval(x);
		// do the inverse transform
		// active transform of points to rotated coordinates
		xRet = ct * x - st * yPoly;
		yRet = st * x + ct * yPoly;

		return yPoly;
	}

	// squared vertical (change in y, not perpendicular) distance
	// between point (x, y) and the polynomial, (x, f(x))
	double sqDistance(double x, double y)
	{
		double yhat = eval(x);
		double diff = yhat - y;
		return diff * diff;
	}


	double rotatedParabolaSqDistance(double x, double y)
	{
		if (degree != 2)   //only works for parabolas
			return -1;
		//rotate point (x, y)
		//double newX = ct * x + st * y;
		//double newY = -st * x + ct * y;
		rotatePointToParabolaCoordinates(x, y);
		return sqDistance(x, y);
	}


	void rotatePointToParabolaCoordinates(double& x, double& y)
	{
		double tempX = x, tempY = y;
		x = ct * tempX + st * tempY;
		y = -st * tempX + ct * tempY;
	}


	// Fits a completely (but not over-) determined polynomial
	// That is, for a polynomial of degree k, k+1 points are needed
	static Polynomial<degree> findPoly(double x[], double fx[])
	{
		SquareMatrix A(degree + 1);
		ColumnVector b(degree + 1);

		for(int i = 0; i < degree + 1; i++) {
			for(int j = 0; j < degree + 1; j++) {
				A.element(i, j) = std::pow(x[i], j);
			}
			b.element(i) = fx[i];
		}

		// column vector poly will have lowest order term's coeff at poly(0)
		ColumnVector poly = A.i() * b;

		Polynomial p;
		for(int i = 0; i < degree + 1; i++) {
			p.coeffs[i] = poly.element(degree - i);
		}

		return p;
	}

	// Fits a polynomial (with template's degree) to the given points
	// System may be overdetermined.
	// x and y are vectors where (x[0], fx[0]) are pairs of points
	static Polynomial<degree> fitRegressionPoly(double x[], double fx[], int N)
	{
		// return value
		Polynomial p;

		if(N < 2) return p;

		double *x_nr = nr::vector(1, N);
		double *y_nr = nr::vector(1, N);
		double *sig_nr = nr::vector(1, N);

		double *a_nr = nr::vector(1, degree + 1);
		int *ia_nr = nr::ivector(1, degree + 1);

		double **covar_nr = nr::matrix(1, degree + 1, 1, degree + 1);

		// fill the variables
		for(int i = 0; i < N; i++) {
			x_nr[i + 1] = x[i];
			y_nr[i + 1] = fx[i];
			sig_nr[i + 1] = 1.0;
		}

		for(int i = 0; i < degree + 1; i++) {
			ia_nr[i + 1] = 1;
		}

		nr::lfit(x_nr, y_nr, sig_nr, N, a_nr, ia_nr, degree + 1, covar_nr, nr::fpoly);

		for(int i = 0; i < degree + 1; i++) {
			p.coeffs[degree - i] = a_nr[i + 1];
		}

		// find min and max x-values to limit the domain
		std::sort(x, x + N);
		p.setMinX(x[(int) (0.05 * N)]);
		p.setMaxX(x[N - 1 - (int) (0.05 * N)]); // handles small (degenerate) N

		nr::free_vector(x_nr, 1, N);
		nr::free_vector(y_nr, 1, N);
		nr::free_vector(sig_nr, 1, N);

		nr::free_ivector(ia_nr, 1, degree + 1);
		nr::free_vector(a_nr, 1, degree + 1);

		nr::free_matrix(covar_nr, 1, degree + 1, 1, degree + 1);

		return p;
	}

private:
	double theta, ct, st;  //angle of x-axis with horizontal, cos(theta), sin(theta)

	double minX, maxX;  // bounds on the domain of the polynomial

};

#endif // POLYNOMIAL_H
