// A unit test for the Polynomial class and fitter

#include "Polynomial.h"
#include "Parabola.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

#define TEST_REGRESSION 0
#define TEST_POLY_DETERMINATION 1

void printUsage(const char *name)
{
	std::cerr << "Usage:" << std::endl
		      << "   " << name << std::endl
			  << "   Reads whitespace delimited points from stdin" << std::endl
			  << "        1.03 321.4" << std::endl
			  << "        452  -1.3e4" << std::endl
			  << "        ..." << std::endl
			  << "   Fits polynomials, evaluates, and prints value for each x to stdout" << std::endl
			  << "        1.03 2 3 4" << std::endl
			  << "        452  5 6 7" << std::endl
			  << "        ..." << std::endl;
}

double randRangedDouble(void) {
	return (rand() - 0.5)*20;
}

int main(int argc, char **argv)
{
	if(TEST_REGRESSION) {
		if(argc > 1) {
			printUsage(argv[0]);
			return 0;
		}

		std::vector<double> xvec, yvec;
		int N = 0;

		std::ifstream israw("xdata.txt");
		std::istream& is = israw >> std::skipws;
		
		while(!is.eof()) {
			double d;
			is >> d;
			xvec.push_back(d);
			is >> d;
			yvec.push_back(d);
			if(is.eof()) break;
			N++;
		}

		israw.close();
		
		double *x = new double[N]; std::copy(xvec.begin(), xvec.end(), x);
		double *y = new double[N]; std::copy(yvec.begin(), yvec.end(), y);

		Polynomial<1> p1 = Polynomial<1>::fitRegressionPoly(x, y, N);
		Polynomial<2> p2 = Polynomial<2>::fitRegressionPoly(x, y, N);
		Polynomial<5> p5 = Polynomial<5>::fitRegressionPoly(x, y, N);

		std::ofstream osraw("outdata.txt");
		std::ostream& os = osraw << std::setprecision(5);

		osraw << "x y1 y2 y5" << std::endl;
		for(int i = 0; i < N; i++) {
			os << x[i] << " " << p1.eval(x[i]) << " " << p2.eval(x[i]) << " " << p5.eval(x[i]) << std::endl;
		}

		osraw.close();
	}

	else if(TEST_POLY_DETERMINATION) {
		// left out for debugging
		//srand(time(NULL));

		std::cout << "Press enter to run test..." << endl;
		while(!cin.eof()) {
			std::string s; // throwaway, just for blocking on user
			std::getline(std::cin, s);

			// test polynomial construction in Polynomial against line
			//   construction in the parabola class
			double x[3]; float xf[3];
			double y[3]; float yf[3];

			//for(int i = 0; i < 3; i++) {
			//	x[i] = xf[i] = randRangedDouble();
			//	y[i] = yf[i] = randRangedDouble();
			//}
			x[0] = xf[0] = -1.0;
			x[1] = xf[1] = 0.0;
			x[2] = xf[2] = 1.0;
			y[0] = yf[0] = 1.0;
			y[1] = yf[1] = 0.0;
			y[2] = yf[2] = 1.0;

			Parabola par = parabolaForPoints(xf, yf);
			Polynomial<2> pol = Polynomial<2>::findPoly(x, y);

			for(int i = 0; i < 3; i++)
				std::cout << setw(5) << par.coeffs[i];
			std::cout << std::endl;
			for(int i = 0; i < 3; i++)
				std::cout << setw(5) << pol.coeffs[i];
			std::cout << std::endl << std::endl;
		}
	}

	return 0;
}
