#pragma once

#include <cxcore.h>


struct Stopline
{
	float  score;
	bool isValid;
	CvRect rect;
	float x, z;
};

inline Stopline Stopline_new(float score, CvRect rect, float x, float z, bool isValid)
{
	Stopline sl;
	sl.rect = rect;
	sl.score = score;
	sl.x = x;
	sl.z = z;
	sl.isValid = isValid;
	return sl;
}

