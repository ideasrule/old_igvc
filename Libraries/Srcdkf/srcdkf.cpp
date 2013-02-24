
/// \file srcdkf.cpp
// Contains an implementation of a square root central difference kalman filter

#include "srcdkf.h"
#include "GPSpositionmeasurementmodel.h"
#include "GPSPositionAndHeadingMeasurementModel.h"
#include "compassmeasurementmodel.h"
#include "VehicleDynamics.h"
#include "Conversions.h"

#include <cmath>

using namespace Pave_Libraries_Conversions;

double subangle(double a, double b)
{
	double r = a - b;

	if(r > Angle::PI)
		return r - 2*Angle::PI;

	if(r < -Angle::PI)
		return r + 2*Angle::PI; 

	return r;
}


Srcdkf::Srcdkf()
{
	//initialize state vector:
	//state is [x,y,heading] (control is speed (from gps right now, but can also be
	// from encoders) and gyro yaw rate.
	//-7.15 is the magnetic variation for rochester, MIs
	x.resize(3); x << 0.0 << 0.0 << 3.14; 
	//initialize covariance of initial state vector
	S.resize(3); S << 2									//x
		           << 0 << 2							//y 
				   << 0 << 0 <<	  .5;					//theta

	// initialize process noise vector
	// TODO: should x/y/theta be correlated?
	v.resize(3); v = 0.0;                           
	Sv.resize(3); Sv << 0.1					    //x (with speed noise)
					 << 0		<< 0.1				//y (with speed noise)
					 << 0		<< 0	<< 0.01;		//theta (with yaw rate noise)
}

void Srcdkf::setState(const ColumnVector &newState)
{
	if(x.nrows() != NUM_STATE_VARS)
		throw exception("Attempted to set state set to improper dimension vector");
	else
		x = newState;
	
}


ReturnMatrix Srcdkf::getState()
{
	ColumnVector copy = x;

	copy.release();
	return copy;
}

void Srcdkf::predict(const ColumnVector &u)
{

	if(x.nrows() != NUM_STATE_VARS)
		throw exception("Predict called with wrong dimension state");

	//Note: This code and the comments in it will make more sense after referring to
	//van der Merwe and Wan, 2004. I follow their notation as best I can.

	//Set scaling factor. Must be >1, and root 3 is optimal for Gaussian priors.
	double h = sqrt(3.0);

	// Calculate sigma points for time-update

	// Augment state with process-noise means, and covariance with process-noise
	// covariance matrix
	ColumnVector xa = x & v;
	LowerTriangularMatrix Sa(S.nrows() + Sv.nrows()); Sa = 0.0;
	Sa.submatrix(1,S.nrows(), 1, S.ncols()) = S;
	Sa.submatrix(S.nrows() + 1, S.nrows() + Sv.nrows(), S.ncols()+1, S.ncols() + Sv.ncols()) = Sv;
	int L = xa.nrows();


	Matrix ones(1,L); ones = 1.0;
	//chiav (chi augmented with v) contains a sigma point in each column.
	//Could optimize slightly by precomputing xa*ones and h*Sa.
	Matrix chiav = xa | (xa*ones+h*Sa) | (xa*ones-h*Sa);

	// Time-update equations

	Matrix chix(x.nrows(),chiav.ncols());

	for( int i = 1; i <= chiav.ncols(); i++)
		chix.column(i) = VehicleDynamics::Predict(chiav.column(i).rows(1,x.nrows()),chiav.column(i).rows(x.nrows()+1,x.nrows() + v.nrows()),u);

	double w0 = (h*h - (double)L)/(h*h);
	double wi = 1/(2*h*h);
	//Form optimal state prediction xpred by weighting each sigma point
	//appropriately and summing.
	x = w0*chix.column(1) + wi*chix.columns(2,chix.ncols()).sum_rows();

	// Form covariance of prediction
	double wc1 = sqrt(1/(4*h*h));
	double wc2 = sqrt((h*h-1)/(4*h*h*h*h));
	Matrix A = wc1*(chix.columns(2,L+1) - chix.columns(L+2,chix.ncols())) |
	           wc2*(chix.columns(2,L+1) + chix.columns(L+2,chix.ncols()) - 2*chix.column(1)*ones);
	
	QRZT(A, S);
	S = -S;

	//cout << "S after prediction: " << endl;
	//cout << S;

	x(3) = fmod(x(3), 2*Angle::PI); //Put heading back in [0,2PI] range
	//cout << x.t();

	if(x.nrows() != 3)
		throw exception("Predict produced wrong dimension state");
}


void Srcdkf::GPSMeasurementNoHeading(const ColumnVector &y)
{
	//Fuse position measurement
	ColumnVector nPos(2); nPos = 0; 
	LowerTriangularMatrix SnPos(2); SnPos << .10			
										  <<  0	<< .10;

	GPSPositionMeasurementModel *p = new GPSPositionMeasurementModel();
	ColumnVector posIsAngle(2); posIsAngle = 0;
	ColumnVector u(1); u = 0; //Unused
	Measurement(y.rows(1,2), posIsAngle, nPos, SnPos, u, p);
	delete p;
}

void Srcdkf::GPSMeasurementWithHeading(const ColumnVector &y)
{
	//Fuse position measurement
	ColumnVector nPos(3); nPos = 0; 
	LowerTriangularMatrix SnPos(3); SnPos << .10    		         //x
										  <<  0	<< .10            //y
										  <<  0 << 0    << 0.10;  //heading

	GPSPositionAndHeadingMeasurementModel *p = new GPSPositionAndHeadingMeasurementModel();
	ColumnVector posIsAngle(3); posIsAngle = 0; posIsAngle(3) = 1;
	ColumnVector u(1); u = 0; //Unused
	Measurement(y.rows(1,3), posIsAngle, nPos, SnPos, u, p);
	delete p;
}



void Srcdkf::CompassMeasurement(const ColumnVector &y)
{
	ColumnVector n(1); n = 0;
	LowerTriangularMatrix SnComp(1); SnComp << .32;

	CompassMeasurementModel *m = new CompassMeasurementModel();
	
	ColumnVector IsAngle(1); IsAngle = 1;

	ColumnVector u(1); u = 0; 
	Measurement(y, IsAngle, n, SnComp, u, m);
	delete m;
}


void Srcdkf::Measurement(const ColumnVector &y, const ColumnVector &isAngle, const ColumnVector &n, const LowerTriangularMatrix Sn, const ColumnVector &u, MeasurementModel *m)
{

	if(x.nrows() != NUM_STATE_VARS)
		throw exception("Measurement called with wrong dimension state");

	// Calculate sigma points for measurement update

	//Set scaling factor. Must be >1, and root 3 is optimal for Gaussian priors.
	const double h = 1.73205081;   //sqrt(3.0)

	//Augment predicted state with observation-noise means, and covariance with 
	//observation-noise covariance matrix
	ColumnVector xa = x & n;

	LowerTriangularMatrix Sa(S.nrows() + Sn.nrows());
	Sa = 0.0;
	Sa.submatrix(1,S.nrows(), 1, S.ncols()) = S;
	Sa.submatrix(S.nrows() + 1, S.nrows() + Sn.nrows(), S.ncols()+1, S.ncols() + Sn.ncols()) = Sn;
	int L = xa.nrows();

	Matrix ones(1,L); ones = 1.0;
	//chian (chi augmented with n) contains a sigma point in each column.
	//Could optimize slightly by precomputing xa*ones and h*Sa.
	Matrix chian = xa | (xa*ones+h*Sa) | (xa*ones-h*Sa);

	//cout << "chian: " << endl << chian << endl;

	
	// Measurement update equations
	Matrix Y(y.nrows(),chian.ncols());
	for( int i = 1; i <= chian.ncols(); i++)
		Y.column(i) = m->measure(chian.column(i).rows(1,x.nrows()),chian.column(i).rows(x.nrows()+1,x.nrows() + n.nrows()),u);


	//cout << "Y: " << endl << Y << endl;

	double w0 = (h*h - (double)L)/(h*h);
	double wi = 1/(2*h*h); 

	//Calculate predicted measurement
	ColumnVector ypred = w0*Y.column(1) + wi*Y.columns(2,Y.ncols()).sum_rows();

	//cout << "Ysum: " << endl << wi*Y.columns(2,Y.ncols()).sum_rows() << endl;
	//cout << "w0: " << w0 << endl;

	//cout << "ypred: " << endl << ypred << endl;

	double wc1 = sqrt(1/(4*h*h));
	double wc2 = sqrt((h*h-1)/(4*h*h*h*h));
	//Form sqrt of covariance of predicted observation
	Matrix C = wc1*(Y.columns(2,L+1) - Y.columns(L+2,Y.ncols())) |
               wc2*(Y.columns(2,L+1) + Y.columns(L+2,Y.ncols()) - 2*Y.column(1)*ones);

	Matrix Ctemp;
	Ctemp << C;

	LowerTriangularMatrix R;
	QRZT(Ctemp, R);	
	Matrix Sy;
	Sy << -R;
	
	//Compute full covariance matrix
	Matrix Pxy = S*C.columns(1,x.nrows()).t();
	//cout << "S: " << endl;
	//cout << S;
	//cout << "Pxy" << endl;
	//cout << Pxy;

	//Calculate Kalman gain using efficient QR least squares instead of matrix inverse
	//Newmat says it does explicitly calculate the inverse, so this is less efficient.
	//Their work around would obfuscate things further, so I've let this inefficiency
	//stand (our state vectors are trivially small).
	//It suggest B*A.i() with (A.t().i()*B.t()).t()
	Matrix K = (Pxy*Sy.t().i()) * Sy.i(); //(Sy.t().i()*(Sy.i()*Pxy.t())).t();


	//Calculate new optimal estimate of the state

	ColumnVector innovation = (y - ypred);
	//cout << "y:         " << endl << y;
	//cout << "ypred:     " << endl << ypred;
	//cout << "innovation:" << endl <<innovation << endl;
	for(int i = 1; i <= innovation.nrows(); i++)
	{
		if(isAngle(i) == 1)
			innovation(i) = subangle(y(i),ypred(i)); //Handle angles
	}

	//cout << "K";
	//cout << K;

	x += K*innovation;

	
	x(3) = fmod(x(3),2*Angle::PI) + (x(3)<0?2*Angle::PI:0); //Put heading back in range [0,2PI]

	//and corresponding sqrt covariance

	//Unfortunately downdate_Cholesky fails sometimes. The
	//below procedure is equivalent.
	//RowVector U = (K * Sy).AsRow(); //Not convinced this will always be a row vector
	//cout << "U: " << endl;
	//cout << U;

	Matrix Q = (S - K*C.columns(1,x.nrows())) | 
				K*C.columns(x.nrows()+1,C.ncols()) |
	            K*(wc2*(Y.columns(2,L+1) + Y.columns(L+2,Y.ncols()) - 2*Y.column(1)*ones));
	
	LowerTriangularMatrix W;
	QRZT(Q, W);	

	S << -W;

	//This is what I had for the downdate:
	//RowVector U = (K * Sy).AsRow(); //Not convinced this will always be a row vector
	//UpperTriangularMatrix M;
	//M << S.t();
	//downdate_Cholesky(M,U);   
	//S << -M.t();

	//cout << "newx: " << x << endl;
	//cout << "newS: " << S << endl;

	if(x.nrows() != 3)
		throw exception("Measurement produced wrong dimension state");

}

void Srcdkf::putAnglesInRange()
{
	x(3) = fmod(x(3),2*Angle::PI) + (x(3)<0?2*Angle::PI:0);
}
