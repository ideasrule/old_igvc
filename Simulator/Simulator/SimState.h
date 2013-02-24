#pragma once



#define WANT_STREAM                  // include.h will get stream fns
#define WANT_MATH                    // include.h will get math fns
#define WANT_FSTREAM
#define WANT_TIME                

#include "newmatap.h"                // need matrix applications

#include "newmatio.h"                // need matrix output routines

#ifdef use_namespace
using namespace NEWMAT;              // access NEWMAT namespace
#endif

class SimState
{
public:
	ColumnVector x;
	ColumnVector ws;

	ColumnVector lastGPS;
	
	LowerTriangularMatrix Sv; //sqrt of process noise covariance (lower triangular cholesky factor)
	LowerTriangularMatrix Sn; //sqrt of gps noise covariance (lower triangular cholesky factor)

	SimState();
};
