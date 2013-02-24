#include "digitalfilter.h"

using std::vector;

DigitalFilter::DigitalFilter(vector<double> &_coeff)
{
    coefficients = _coeff;
    bufferSize = (int)_coeff.size();
    circBuffer = new double[bufferSize]();
    bufferPtr = circBuffer;
}



DigitalFilter::~DigitalFilter()
{
    delete circBuffer;
}


void DigitalFilter::push_back(double _data)
{
    *bufferPtr = _data;

    if (bufferPtr - circBuffer + 1 < bufferSize) 
        ++bufferPtr;
    else
        bufferPtr = circBuffer;
}


double DigitalFilter::filter()
{
    double y = 0;
    for (int i = 0; i < bufferSize; ++i) {
        if (bufferPtr - circBuffer > 0)
            --bufferPtr;
        else
            bufferPtr = circBuffer + bufferSize - 1;

        y += (*bufferPtr) * coefficients[i];
    }
    return y;
}


double DigitalFilter::update(double _data)
{
    push_back(_data);
    return filter();
}