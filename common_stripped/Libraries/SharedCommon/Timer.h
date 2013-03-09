#pragma once

namespace Pave_Libraries_Common
{
	struct TimerHandlerData
	{
		bool withData;
		void (*timerHandlerNoData)();
		void (*timerHandlerWithData)(void *data);
		void *data;
	};

	class Timer
	{
	public:
		static bool addTimer(void (*timerHandler)(), unsigned long ms);
		static bool addTimer( void (*timerHandler)(void *data), unsigned long ms, void *data);
	private:
		static void timerCallback(void *data, unsigned long a, unsigned long b);
	};
}