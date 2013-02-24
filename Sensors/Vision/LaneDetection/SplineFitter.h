#ifndef SPLINE_FITTER_H
#define SPLINE_FITTER_H

#include "Spline.h"
#include <vector>
#include <list>
#include "cv.h"
#include "Frame.h"

#include "Memory.h"

class SplineFitter {
    
public:

	/*
	class RowMajorPointComparator
	{
	public:
		bool operator() (const cv::Point& lhs, const cv::Point& rhs) const
		{ return (lhs.y < rhs.y) ? true : (lhs.x < rhs.x); }
	};
	*/

    std::vector<float> cumsum;
    cv::Mat_<float> img;
	//std::set<cv::Point,RowMajorPointComparator> imgPts;
	std::list<cv::Point> imgPts;
	cv::Mat_<float> splineImg;
	cv::Mat_<float> multImg;
	
    int numPoints;
	int nIterations;

	// Create a new SplineFitter for each image to be processed 
	SplineFitter(const cv::Mat_<float> &m);

	Spline fitOne(bool zeroOut = true);
    
	static Spline_<float> transformToGround(Spline s, 
		shared_ptr<Pave_Libraries_Camera::Frame> frm);

	// Up to (2*GTPR)^2 cloud points will be considered
	// when transforming each pixel to the ground plane.
	static const int GROUND_TRANSFORM_PIXEL_RADIUS = 5;

private:

	cv::Point sample() const;
	/* Sample a point at a given radius from the center,
	   but not one that is close to the third argument,
	   if it is not the center */
	cv::Point sampleAtRadius(cv::Point center, float radius, 
		cv::Point negWeight) const;
	Spline sampleSpline() const;
	void updateCumsum();
	void displayImgPts(const char *windowName) const;
};

#endif
