#pragma once
#include <iostream>
#include <cmath>

using namespace std;

namespace Pave_Libraries_Conversions
{
	class Angle {
	public:
		static double DEG_RAD(double aDeg);
		static double RAD_DEG(double aRad);
		static const double PI;
		static const double DegPerRad;
	};

	class Distance {
	public:
		static double T_M(double t);
		static double T_MI(double t);
		static double IN_M(double dIn);
		static double IN_MI(double dIn);
		static double M_FT(double dM);
		static double M_MI(double dM);
		static double KM_MI(double dKm);
		static double FT_MI(double dFt);
		static double FT_M(double dFt);
		static double MI_KM(double dMi);
		static double MI_M(double dMi);
		static double MI_FT(double dMi);
	private:
		static const double FtPerM;
		static const double FtPerMi;
		static const double MPerMi;
		static const double InPerM;
		static const double InPerMi;
		static const double TPerRev;
		static const double Dia;
	};

	class Speed {
	public:
		static double MPerSec_MiPerH(double sMps);
		static double MiPerH_MPerSec(double sMph);
		static double Kt_MPerSec(double dKnots);
		static double TPerSec_MiPerH(double tps);
		static double TPerSec_MPerSec(double tps);
	private:
		static const int SecPerHr = 3600;
	};

	class Temperature {
	public:
		static double C_K(double tC);
		static double K_C(double tK);
		static double C_F(double tC);
		static double F_C(double tF);
	private:
		static const double KCConst;
		static const double FCConst;
		static const double FCFactor;
	};
}