#pragma once

#include "Windows.h"

namespace Pave_OPEC_PID
{
   class PID
   {
   public:
      PID();
      PID(double p, double i, double d);
      void setDesired(double desired);
      double getOutput(double actual);
	  void resetIterm(void);

	  //private:
	  double p;
      double i;
      double d;
      double iError;
      double lastError;
      double desired;
	  double lastOutput;;
	  static const double maxChange;
	  bool hasLimited;
	  LARGE_INTEGER lastTicks;
	  LARGE_INTEGER frequency;
	  static const double zeroThreshold;
   };
}