#include "MedianFilter.h"


namespace Pave_Libraries_Common
{
	MedianFilter::MedianFilter()
	{
		this->length = 1;
		this->values = new double[length];
		this->sortedValues = new double[length];
	}

	MedianFilter::MedianFilter(int numValues)
	{
		this->length = numValues;
		this->values = new double[numValues];
		this->sortedValues = new double[numValues];
		for(int i = 0; i < this->length; i++)
		{
			values[i] = 0;
			sortedValues[i] = 0;
		}
	}
		
	double MedianFilter::getMedian(void)
		{
			for(int i = 0; i < this->length;i++)
			{
				sortedValues[i] = values[i];
			}
			sort(this->sortedValues,this->sortedValues+this->length);
			return sortedValues[(int) length/2];
		}

	void MedianFilter::addValue(double newVal)
	{
			for(int i = 0; i < this->length-1;i++)
			{
				values[i] = values[i+1];
			}
			values[length-1] = newVal;
		}
}