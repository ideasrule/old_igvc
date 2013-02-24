#pragma once


//#define WANT_STREAM                  // include.h will get stream fns
//#define WANT_MATH                    // include.h will get math fns
//#define WANT_FSTREAM
//#define WANT_TIME                
//
//#include "newmatap.h"                // need matrix applications
//
//#include "newmatio.h"                // need matrix output routines
//
//#ifdef use_namespace
//using namespace NEWMAT;              // access NEWMAT namespace
//#endif


namespace Pave_Libraries_Navigation
{
class VehicleDynamics
{
public:
	static const double Length;
	static const double Width;
	// The distance the reference point is from the front
	static const double ReferenceOffset;
	static const double Wheelbase;
	
	//x is a column vector consisting of [x y heading], v is noise [V omega]', and u is input [vr vl]'
	//returns predicted new x
	 //ReturnMatrix Predict(const ColumnVector& x, const & ColumnVector& v, const ColumnVector &u);
};
}
/*
const double VehicleDynamics::Width = 1;
const double VehicleDynamics::Length = 2;
const double VehicleDynamics::ReferenceOffset = 1;*/