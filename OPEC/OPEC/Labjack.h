#pragma once

#include "LabjackUD.h"

//For PWM signals, we want a ~60Hz signal
#define LJ_CLOCK LJ_tcSYS
#define CLOCK 48000000
#define DIVISOR 12
#define PWMMode LJ_tmPWM16
#define	PWMBits 16

namespace Pave_OPEC_Labjack
{
	struct LJInputs
	{
      long Timer_0; // left wheel quadrature count // right wheel duty cycle
      //long Timer_3; //same as Timer_2
	  long Timer_2; // right wheel quadrature count
	  //long Timer_5; // same as Timer_4
	
	  double AIN_0; // Gladiator Gyro
	  double AIN_1; // Gladiator Gyro
	  bool MIO_0; // e-stop in
	  bool MIO_1; // run/pause in
	};

	struct LJOutputs
	{
		double Timer_4; // left wheel PWM
		bool send_Timer_4;

		double Timer_5; // right wheel PWM
		bool send_Timer_5;

		int MIO_2; // light/siren
		bool send_MIO_2;
	};

	struct LabjackIO
	{
	  static const long FIO = 0;
	  static const long EIO = 8;
	  static const long CIO = 16;
	  static const long MIO = 20;
	};

	class Labjack
	{
	public:
		Labjack(); // USB
		Labjack(const char *ip); // ETHERNET
		static const LabjackIO ports;
		struct LJInputs *readInputs();
		void sendOutputs(LJOutputs *outputs);
		void resetLabjack();
	private:
		struct LJInputs inputs;
		struct LJOutputs outputs;
		LJ_HANDLE lj;
		void checkError(LJ_ERROR error);
		void initialize();
		long PWM(double percent);
		double PWMperiod;
	};
	
}