#include "Conversions.h"

namespace Pave_Libraries_Conversions
{
	const double Angle::PI = 4.0*atan(1.0);
	const double Angle::DegPerRad = 180 / PI;

	double Angle::DEG_RAD(double aDeg) {return aDeg / DegPerRad;}
	double Angle::RAD_DEG(double aRad) {return aRad * DegPerRad;}

	const double Distance::FtPerM  = 3.2808399;
	const double Distance::FtPerMi = 5280;
	const double Distance::MPerMi  = 1609.344;
	const double Distance::InPerM  = 39.3700787;
	const double Distance::InPerMi = 63360;
	const double Distance::TPerRev = 8192;
	//const double Distance::Dia = 11.66; //calibrated 5/21/09 GHF & ANT
	const double Distance::Dia = 11.835; //calibrated 5/30/10 TZ

	double Distance::FT_M(double dFt)  { return dFt / FtPerM; }
	double Distance::FT_MI(double dFt) { return dFt / FtPerMi; }
	double Distance::IN_M(double dIn)  { return dIn / InPerM; }
	double Distance::IN_MI(double dIn) { return dIn / InPerMi; }
	double Distance::KM_MI(double dKm) { return M_MI(dKm * 1000); }
	double Distance::M_FT(double dM)   { return dM * FtPerM; }
	double Distance::M_MI(double dM)   { return dM / MPerMi; }
	double Distance::MI_FT(double dMi) { return dMi * FtPerMi; }
	double Distance::MI_KM(double dMi) { return MI_M(dMi) / 1000; }
	double Distance::MI_M(double dMi)  { return dMi * MPerMi; }
	double Distance::T_M(double t)     { return (t / (TPerRev)) * IN_M(Dia) * Angle::PI; }
	double Distance::T_MI(double t)    { return (t / (TPerRev)) * IN_MI(Dia) * Angle::PI; }//{ return (t / TPerRev) * IN_MI(Dia); }

	double Speed::Kt_MPerSec(double dKnots)        { return dKnots * (0.5144444444); }
	double Speed::MiPerH_MPerSec(double sMph) { return sMph * (Distance::MI_M(1) / SecPerHr); }
	double Speed::MPerSec_MiPerH(double sMps) { return sMps * (SecPerHr * Distance::M_MI(1)); }
	double Speed::TPerSec_MiPerH(double tps)  { return tps * SecPerHr * Distance::T_MI(1); }
	double Speed::TPerSec_MPerSec(double tps) { return tps * Distance::T_M(1); }

	const double Temperature::FCConst  = 32;
	const double Temperature::FCFactor = 5 / 9;
	const double Temperature::KCConst  = 273.15;

	double Temperature::C_K(double tC) { return tC + KCConst; }
	double Temperature::K_C(double tK) { return tK - KCConst; }
	double Temperature::C_F(double tC) { return (tC / FCFactor) + FCConst; }
	double Temperature::F_C(double tF) { return (tF - FCConst) * FCFactor; }
}