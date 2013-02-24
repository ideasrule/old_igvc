// Implements a class for digital filtering
// -tzhu

#pragma once
#include <vector>

class DigitalFilter
{
public:
    DigitalFilter(std::vector<double> &_coeff);
    ~DigitalFilter();

    void push_back(double _data);
    double filter();
    double update(double _data);   //for convenience, combine push_back and filter

private:
    std::vector<double> coefficients;   // these are the b coefficients (computed by Matlab)
    int bufferSize;
    double *circBuffer;
    double *bufferPtr;    //current location of the buffer

};
