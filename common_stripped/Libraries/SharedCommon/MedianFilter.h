#pragma once
#include <algorithm>
using namespace std;

namespace Pave_Libraries_Common
{
	class MedianFilter
	{
	public:
		MedianFilter();
		MedianFilter(int numValues);
		double getMedian(void);
		void addValue(double newValue);
		
	private:
		int length;
		double* values;
		double* sortedValues;

	};
}