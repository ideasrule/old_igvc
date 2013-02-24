#pragma once

#define WANT_STREAM                  // include.h will get stream fns
#define WANT_MATH                    // include.h will get math fns
#define WANT_FSTREAM
#define WANT_TIME

#define NUM_STATE_VARS 3


#include "newmatap.h"                // need matrix applications

#include "newmatio.h"                // need matrix output routines

#ifdef use_namespace
using namespace NEWMAT;              // access NEWMAT namespace
#endif

#include "measurementmodel.h"

//An efficient SRCDKF implementation following Van Der Merwe, 2004.
//Currently hard coded for the dynamics of a differential drive robot.
//Publishes a new StateEstimationType message on every prediction cycle.

class Srcdkf
{
private:
	ColumnVector x; //State -- [x; y; heading]
	LowerTriangularMatrix S; // sqrt of covariance of state error (lower triangular cholesky factor)
	ColumnVector v; //Process noise mean
	LowerTriangularMatrix Sv; //Process noise sqrt covariance (lower triangular cholesky factor)
	
	//Measurement noise are assigned individually in the measure functions
	//ColumnVector n; //Measurement noise mean
	//LowerTriangularMatrix Sn; //Measurement noise sqrt covariance (lower triangular cholesky factor)

public:
	//Initializes the Srcdkf
	Srcdkf();

	//Ignore everything and set the state to the provided values.
	//Good for initializing things when GPS first acquires its fix, etc.
	void setState(const ColumnVector &newState);

	ReturnMatrix getState();
	void predict(const ColumnVector &u);
	void Measurement(const ColumnVector &y, const ColumnVector &isAngle, const ColumnVector &n, const LowerTriangularMatrix Sn, const ColumnVector &u, MeasurementModel *m);

	void GPSMeasurementNoHeading(const ColumnVector &y);
	void GPSMeasurementWithHeading(const ColumnVector &y);
	void CompassMeasurement(const ColumnVector &y);

	void putAnglesInRange();
};
