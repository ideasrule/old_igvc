#pragma once

#include "ImageLane.h"
#include "Frame.h"
#include <stdio.h>
#include <vector>
using namespace std;

class Validator
{
public:
	Validator() { }
	void validateLanes(vector<ImageLane *>& detectedLanes, vector<ImageLane *>& validatedLanes);
	//void handValidateLanes(IplImage *mono, IplImage *color, vector<ImageLane * > &lanes, FILE * f, char *name);
	~Validator() { }
private:

};