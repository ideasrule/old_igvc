/*----------------------------------------------------------------------------
	A Y R W A R E   C O N F I D E N T I A L   P R O P R I E T A R Y


	COPYRIGHT (C)2006, All Rights Reserved

	Author:			JHC

	Company:		ayrware

	Description:	PerformanceTimer.h


	Revision History:


	$Log:  $


----------------------------------------------------------------------------*/

#pragma once

class CPerformanceTimer
{
	public:
		CPerformanceTimer();
		~CPerformanceTimer();

		bool Start();
		bool Stop();
		void Reset();

		double Interval_S();
		double Interval_mS();
		double Interval_uS();

		double Resolution_S();
		double Resolution_mS();
		double Resolution_uS();
		double Correction_uS();

	private:
		void Initialise();

	private:
		__int64 m_Interval;
		__int64 m_Frequency;
		__int64 m_startCount;
		__int64 m_stopCount;
		__int64 m_adjustCount;
		bool m_bTimerSupported;
		bool m_bTimerRunning;
};
