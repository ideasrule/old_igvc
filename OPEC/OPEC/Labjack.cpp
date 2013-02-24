#include "Labjack.h"
#include "LabjackUD.h"
#include <stdio.h>
#include <ostream>
#include <iostream>
#include <fstream>
#include <cmath>
using namespace std;

namespace Pave_OPEC_Labjack
{
	Labjack::Labjack()
	{
		checkError(OpenLabJack(LJ_dtUE9, LJ_ctUSB, "0", 1, &lj));
		initialize();
	}

	Labjack::Labjack(const char *ip)
	{
		//192.168.1.251 (Jill borrowed from P12)
		checkError(OpenLabJack(LJ_dtUE9, LJ_ctETHERNET, ip, 0, &lj));
		initialize();
	}

	void Labjack::initialize()
	{
		this->PWMperiod = 1/((CLOCK*1.0/DIVISOR*1.0)/(pow(2.0,PWMBits)));
		
		double version = GetDriverVersion();
		if(version != DRIVER_VERSION)
		{
			cout << "Incompatible Driver!" << endl;
		}
		checkError(AddRequest(lj, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_BASE, LJ_CLOCK, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_CONFIG, LJ_chTIMER_CLOCK_DIVISOR, DIVISOR, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 6, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, 14, 0, 0));
		checkError(GoOne(lj));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 0, LJ_tmQUAD, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 1, LJ_tmQUAD, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 2, LJ_tmQUAD, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 3, LJ_tmQUAD, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 4, PWMMode, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 5, PWMMode, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_AIN_RANGE, 0, LJ_rgUNI5V, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_AIN_RANGE, 1, LJ_rgUNI5V, 0, 0));
		checkError(GoOne(lj));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 1, 0, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 2, 0, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 3, 0, 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 4, PWM(0), 0, 0));
		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 5, PWM(0), 0, 0));   
		//checkError(AddRequest(lj, LJ_ioPUT_DIGITAL_BIT, ports.MIO + 2, 0, 0, 0));
		//checkError(AddRequest(lj, LJ_ioGET_TIMER_MODE, 4,0,0,0));
		checkError(GoOne(lj));
		//double val;
		//checkError(GetResult(lj, LJ_ioGET_TIMER_MODE, 4, &val));
		//cout << val << endl;

		// Some bullshit
		//checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 4, PWMMode, 0, 0));
		//checkError(AddRequest(lj, LJ_ioPUT_TIMER_MODE, ports.FIO + 5, PWMMode, 0, 0));
		//checkError(GoOne(lj));
		//while(1)
		//	for(int i = 0; i < 10000000000; i++)
		//		if( i%10000 == 0)
		//		{
		//checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 4, 15000, 0, 0));
		//checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 5, 15000, 0, 0));
		//			checkError(GoOne(lj));
		//		}
   }

	void Labjack::sendOutputs(LJOutputs *out)
	{
// Some more bullshit

//		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 4, 30000, 0, 0));
//		checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 5, 30000, 0, 0));
//checkError(GoOne(lj));
//		return;


		bool send = true;
		LJOutputs &outputs = *out;

      // add requests
		//checkError(AddRequest(lj,LJ_ioGET_TIMER_MODE,4,0,0,0));
		//checkError(AddRequest(lj,LJ_ioGET_TIMER_MODE,5,0,0,0));
		
		if(outputs.send_Timer_4)//&& (outputs.Timer_4 != this->outputs.Timer_4))
		{
			long t4out = PWM(outputs.Timer_4);
			checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 4, t4out, 0, 0));
			this->outputs.Timer_4 = outputs.Timer_4;
			send = true;
			// cout << endl << t4out << "\t";
		}
		if(outputs.send_Timer_5)//&& (outputs.Timer_5 != this->outputs.Timer_5))
		{
			long t5out = PWM(outputs.Timer_5);
			checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 5, t5out, 0, 0));
			this->outputs.Timer_5 = outputs.Timer_5;
			send = true;
			// cout << t5out << endl;
		}
		if(outputs.send_MIO_2 && (outputs.MIO_2 != this->outputs.MIO_2))
		{
			checkError(AddRequest(lj, LJ_ioPUT_DIGITAL_BIT, ports.MIO + 2, outputs.MIO_2, 0, 0));
			this->outputs.MIO_2 = outputs.MIO_2;
			send = true;
		}
      
      // send
      if(send)
		checkError(GoOne(lj));

	  /*double val;
	  checkError(GetResult(lj,LJ_ioGET_TIMER_MODE,4,&val));
	  cout << endl << val << "\t";
	  checkError(GetResult(lj,LJ_ioGET_TIMER_MODE,5,&val));
	  cout << val << endl;*/

	}

	LJInputs *Labjack::readInputs()
   {
	   // Extra bullshit
	   //return &inputs;

      /* send requests */
	  //timer values for quad count
      checkError(AddRequest(lj, LJ_ioGET_TIMER, 0, 0, 0, 0));
      //checkError(AddRequest(lj, LJ_ioGET_TIMER, 1, 0, 0, 0));
	  checkError(AddRequest(lj, LJ_ioGET_TIMER, 2, 0, 0, 0));
      //checkError(AddRequest(lj, LJ_ioGET_TIMER, 3, 0, 0, 0));      
	  //digital IO
      checkError(AddRequest(lj, LJ_ioGET_DIGITAL_BIT, ports.MIO + 0, 0, 0, 0));
      checkError(AddRequest(lj, LJ_ioGET_DIGITAL_BIT, ports.MIO + 1, 0, 0, 0));
	  // Reset the quadrature count (call returns last count)
	  checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 0, 0, 0, 0));
	  checkError(AddRequest(lj, LJ_ioPUT_TIMER_VALUE, 2, 0, 0, 0));
	  // Analog inputs from gyro
	  checkError(AddRequest(lj, LJ_ioGET_AIN, 0, 0, 0, 0));
      checkError(AddRequest(lj, LJ_ioGET_AIN, 1, 0, 0, 0));
      checkError(GoOne(lj));


      // read requests
      double val;
      checkError(GetResult(lj, LJ_ioGET_TIMER, 0, &val));
      inputs.Timer_0 = (long)val;
      //checkError(GetResult(lj, LJ_ioGET_TIMER, 1, &val));
      //inputs.Timer_3 = (long)val;
	  checkError(GetResult(lj, LJ_ioGET_TIMER, 2, &val));
      inputs.Timer_2 = (long)val;
	  //checkError(GetResult(lj, LJ_ioGET_TIMER, 3, &val));
      //inputs.Timer_5 = (long)val;
      checkError(GetResult(lj, LJ_ioGET_DIGITAL_BIT, ports.MIO + 0, &val));
      inputs.MIO_0 = (val != 0);
	  //cout << "MIO_0: " << val << "\t";
	  checkError(GetResult(lj, LJ_ioGET_DIGITAL_BIT, ports.MIO + 1, &val));
	  //cout << "MIO_1: " << val << endl;
      inputs.MIO_1 = (val != 0);
	  checkError(GetResult(lj, LJ_ioGET_AIN, 0, &val));
	  inputs.AIN_0 = val;
	  checkError(GetResult(lj, LJ_ioGET_AIN, 1, &val));
	  inputs.AIN_1 = val;
      return &inputs;
   }

	void Labjack::resetLabjack()
   {
      checkError(AddRequest(lj, LJ_ioPUT_CONFIG, LJ_chNUMBER_TIMERS_ENABLED, 0, 0, 0));
      checkError(AddRequest(lj, LJ_ioPUT_COUNTER_ENABLE, 0, 0, 0 , 0));
      checkError(AddRequest(lj, LJ_ioPUT_COUNTER_ENABLE, 1, 0, 0 , 0));
	  checkError(AddRequest(lj, LJ_ioPUT_DIGITAL_BIT, ports.MIO + 1, 0, 0, 0));
      checkError(GoOne(lj));
   }	
	void Labjack::checkError(LJ_ERROR error)
	{
		if(error != LJE_NOERROR)
		{
			char msg[255];
			ErrorToString(error, msg);
			printf("LABJACK ERROR: %d=%s\n",error,msg);
		}
	}

	
	long Labjack::PWM(double percent)
	{
		//bounds checking; percent must be between -1 and 1
		if(percent > 1)
		  percent = 1;
		if(percent < -1)
		  percent = -1;

		//set deadband percent
		double deadband = 0.01;
		//set neutral PWM high-time;
		double milli = .0015; 

		//PWM high-time constants taken 5/15/09 - GHF
		//full reverse = 0.995 ms (left)	2.065 ms (right)
		//idle reverse = 1.443 ms (left)	1.570 ms (right)
		//idle forward = 1.555 ms (left)	1.455 ms (right)
		//full forward = 2.065 ms (left)	0.995 ms (right)
		

		if(percent > deadband)
		{
			milli = .001557 + percent * (0.001988-0.001557);
		}
		if(percent < -deadband)
		{
		   milli = .001443 + percent * (0.001443-0.0010846);
		}
	   
		return (long)(65536 * (1 - (milli/this->PWMperiod)));
	}

}
