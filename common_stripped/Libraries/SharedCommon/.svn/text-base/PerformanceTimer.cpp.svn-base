/*----------------------------------------------------------------------------
	A Y R W A R E   C O N F I D E N T I A L   P R O P R I E T A R Y


	COPYRIGHT (C)2006, All Rights Reserved

	Author:			JHC

	Company:		ayrware

	Description:	PerformanceTimer.cpp


	Revision History:


	$Log:  $


----------------------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "Performancetimer.h"
#include <cmath>


/*============================================================================

	CPerformanceTimer construction / destruction

============================================================================*/

CPerformanceTimer::CPerformanceTimer()
{
	this->Initialise();
}

CPerformanceTimer::~CPerformanceTimer()
{
}


/*============================================================================

	CPerformanceTimer public member functions

============================================================================*/

/*----------------------------------------------------------------------------
	Function:	Start
	Purpose:	Simply starts the timer running
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		bool bSuccess, operation status
----------------------------------------------------------------------------*/
inline bool CPerformanceTimer::Start()
{
	bool bSuccess = false;

	if(!m_bTimerRunning && m_bTimerSupported)
	{
		m_startCount = 0;
		m_stopCount = 0;
		m_Interval = 0;

		if(::QueryPerformanceCounter((LARGE_INTEGER*)&m_startCount))
		{
			m_bTimerRunning = true;
			bSuccess = true;
		}
	}

	return bSuccess;
}

/*----------------------------------------------------------------------------
	Function:	Stop
	Purpose:	Halts the timer if it is currently running
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		bool bSuccess, operation status
----------------------------------------------------------------------------*/
inline bool CPerformanceTimer::Stop()
{
	bool bSuccess = false;

	if(m_bTimerRunning && m_bTimerSupported)
	{
		if(::QueryPerformanceCounter((LARGE_INTEGER*)&m_stopCount))
		{
			m_bTimerRunning = false;
			bSuccess = true;
		}
	}

	return bSuccess;
}

/*----------------------------------------------------------------------------
	Function:	Reset
	Purpose:	Sets timer member variables to default values
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		None
----------------------------------------------------------------------------*/
void CPerformanceTimer::Reset()
{
	this->Initialise();
}

/*----------------------------------------------------------------------------
	Function:	Interval_S
	Purpose:	Allows clients to obtain the current time interval
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured time interval (seconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Interval_S()
{
	return ((double)(m_stopCount - m_startCount) - m_adjustCount) / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Interval_mS
	Purpose:	Allows clients to obtain the current time interval
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured time interval (milliseconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Interval_mS()
{
	return (((m_stopCount - m_startCount) - m_adjustCount) * 1000.0) / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Interval_uS
	Purpose:	Allows clients to obtain the current time interval
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured time interval (microseconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Interval_uS()
{
	return (((m_stopCount - m_startCount) - m_adjustCount) * 1000000.0) / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Resolution_S
	Purpose:	Allows clients to obtain the timer's resolution
	Comments:	Due to the nature of the operating system in which this
				application runs, the calculated timer resolution may
				not be achievable in practice.
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured resolution (seconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Resolution_S()
{
	return 1.0 / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Resolution_mS
	Purpose:	Allows clients to obtain the timer's resolution
	Comments:	Due to the nature of the operating system in which this
				application runs, the calculated timer resolution may
				not be achievable in practice.
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured resolution (milliseconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Resolution_mS()
{
	return 1000.0 / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Resolution_uS
	Purpose:	Allows clients to obtain the timer's resolution
	Comments:	Due to the nature of the operating system in which this
				application runs, the calculated timer resolution may
				not be achievable in practice.
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured resolution (microseconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Resolution_uS()
{
	return 1000000.0 / (double)m_Frequency;
}

/*----------------------------------------------------------------------------
	Function:	Correction_uS
	Purpose:	Allows clients to determine what the start/stop correction is
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		measured correction (microseconds)
----------------------------------------------------------------------------*/
double CPerformanceTimer::Correction_uS()
{
	return (m_adjustCount * 1000000.0) / (double)m_Frequency;
}


/*============================================================================

	CPerformanceTimer private member functions

============================================================================*/

/*----------------------------------------------------------------------------
	Function:	Initialise
	Purpose:	Initialises member variables, calculates the timer correction
	Comments:	None
------------------------------------------------------------------------------
	Parameter[IN]:	None
	Parameter[OUT]:	None
	Returns:		None
----------------------------------------------------------------------------*/
void CPerformanceTimer::Initialise()
{
	m_Frequency = 0;
	m_adjustCount = 0;
	m_bTimerSupported = false;
	m_bTimerRunning = false;

	if(::QueryPerformanceFrequency((LARGE_INTEGER*)&m_Frequency))
	{
		m_bTimerSupported = true;

		// Measure the 'Stop' function call overhead
		const int iNumSamples = 10;
		__int64 Samples[iNumSamples];
		__int64 CountTot = 0;
		double dAvCount = 0.0;
		double dAvDeviance = 0.0;

		for(int i = 0; i < iNumSamples; i++)
		{
			this->Start();
			this->Stop();

			Samples[i] = m_stopCount - m_startCount;
			CountTot += Samples[i];
		}

		dAvCount = (double)CountTot / (double)iNumSamples;

		// Get the average deviance
		for(int i = 0; i < iNumSamples; i++)
		{
			dAvDeviance += fabs(((double)Samples[i]) - dAvCount);
		}

		// Average deviance only required for debug
		dAvDeviance /= iNumSamples;
		m_adjustCount = (__int64)dAvCount;
	}
}


