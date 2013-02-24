#include "PID.h"
#include <stdio.h>
#include <iostream>
#include "Common.h"

namespace Pave_OPEC_PID
{
	//class-wide variables
	const double PID::zeroThreshold = 0.05;
	const double PID::maxChange = .3;

	PID::PID()
	{
		p = 0;
		i = 0;
		d = 0;
		desired = 0;
		lastError = 0;
		iError = 0;
		lastOutput = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTicks);
		hasLimited = false;
	}
	PID::PID(double p, double i, double d)
	{
		this->p = p;
		this->i = i;
		this->d = d;
		desired = 0;
		lastError = 0;
		iError = 0;
		lastOutput = 0;
		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&lastTicks);
		hasLimited = false;
	}
	void PID::setDesired(double des)
	{
		desired = des;
	}

	double PID::getOutput(double actual)
	{
		LARGE_INTEGER currentTicks;
		QueryPerformanceCounter(&currentTicks);
		double elapsed = (double)(((long double)currentTicks.QuadPart - (long double)lastTicks.QuadPart)/(long double)frequency.QuadPart);

		double error = desired - actual;
		double dError = (error - lastError) / elapsed; 
		
		/*if(hasLimited)
			hasLimited = false;
		else */
			iError += error * elapsed; 

		iError = min(max(iError,-1/i),1/i);
		
		if(desired == 0 && abs(actual) < zeroThreshold) 
			resetIterm();
		
		lastError = error;
		lastTicks = currentTicks;
		//cout << error << " actual: " << actual << " desired: "<<desired<<"\r\n";
		double output = p * error + i * iError + d * dError;
		
		//if(output > lastOutput + maxChange)
		//{
		//	output = lastOutput + maxChange;
		//	hasLimited = true;
		//}
		//else if (output < lastOutput - maxChange)
		//{
		//	output = lastOutput - maxChange;
		//	hasLimited = true;
		//}
		
		return output;
		lastOutput = output;
	}


	void PID::resetIterm(void)
	{
		this->iError = 0;
	}
}