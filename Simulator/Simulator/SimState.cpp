#pragma once

#include "SimState.h"

SimState::SimState()
{
	x.resize(7); x = 0.0;
	lastGPS.resize(4); lastGPS = 0.0;

	Sv.resize(3); Sv = 0.0;
	Sn.resize(4); Sn = 0.0;

}