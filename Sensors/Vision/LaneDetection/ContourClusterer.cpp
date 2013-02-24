#include "ContourClusterer.h"
#include <assert.h>
#include <algorithm>
#include "PerfTimer.h"
#include <limits>
#include <boost/format.hpp>

using namespace cv;
using namespace std;

#define MIN_CLUSTER_SIZE 3000

static const int SMOOTHINGS[] = {
	0,4,8
};
#define NSMOOTHINGS 3

const int ContourClusterer::num_smoothings = NSMOOTHINGS;

ContourClusterer::ContourClusterer(const cv::Mat_<float> &img, 
				 const OpenCV2BoxFilter &boxFilter, 
				 int singleSmoothing)
: Clusterer(img), boxFilter(boxFilter),
currSmoothing(singleSmoothing < 0 ? 0 : singleSmoothing), 
restrictCurrSmoothing(singleSmoothing >= 0),
iContour(0), contours(), boxFiltered()
{
}

ContourClusterer::~ContourClusterer()
{
}

shared_ptr<Cluster> ContourClusterer::emitCluster()
{
	while (currSmoothing < NSMOOTHINGS)
	{
		// do we need to recalculate?
		while (iContour >= contours.size())
		{
			if (currSmoothing >= NSMOOTHINGS)
				return shared_ptr<Cluster>();

			cout << "Reblurring at smoothing # " << currSmoothing << endl;

			// Create a blurred version
			{
				PerfTimer thisBlock("Blurring");
				int filterKernelSize = SMOOTHINGS[currSmoothing];
				Rect filterKernel(-filterKernelSize, -filterKernelSize,
					2*filterKernelSize+1,
					2*filterKernelSize+1);
				boxFilter.setBoxFilter(boxFiltered32, filterKernel);
				boxFiltered32.convertTo(boxFiltered, boxFiltered.type());
			}

			// Find contours
			{
				PerfTimer thisBlock("findContours");
				contours.clear();
				findContours(boxFiltered, contours, 
					CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1);
				iContour = 0;
			}

			if (iContour >= contours.size())
			{
				cout << "Ended smoothing # " << currSmoothing << endl;
				if (!restrictCurrSmoothing) ++currSmoothing;
				else currSmoothing = NSMOOTHINGS; // so we return NULL on future calls
			}

			cout << "Done reblurring and finding contours." << endl;
		}

		// try to find a big enough contour in our list
		// and break out of this
		while (iContour < contours.size())
		{
			// Extract the current contour
			const vector<Point> &contour = contours[iContour];
			++iContour;
			if (iContour >= contours.size())
			{
				cout << "Ended smoothing # " << currSmoothing << endl;
				if (!restrictCurrSmoothing) ++currSmoothing;
				else currSmoothing = NSMOOTHINGS; // so we return NULL on future calls
			}

			// If we've found something sizeable
			const Mat contourMat(contour);
			const Rect bounds = cv::boundingRect(contourMat);
			//const int area = (int)cv::contourArea(contourMat);
			const int area = bounds.area();
			if (area >= MIN_CLUSTER_SIZE)
			{
				Point lowest = *(std::min_element(
					contour.begin(), contour.end(), 
					Cluster::PointCompare()));
				Point highest = *(std::max_element(
					contour.begin(), contour.end(), 
					Cluster::PointCompare()));
				vector<Point> deepest(2);
				deepest[0] = lowest;
				deepest[1] = highest;
				/*
				return shared_ptr<Cluster>(new ContourCluster(
					contourMat, area, bounds, deepest, 
					SMOOTHINGS[currSmoothing]));
				*/

				{
					string s = str(boost::format("Creating cluster with area %1%, %2% contour pts") % area % contour.size());
					PerfTimer thisBlock(s);
					Mat_<unsigned char> drawnContour(bounds.size(), 0);
					cv::drawContours(drawnContour, contours, iContour-1, Scalar(1), CV_FILLED, 
						/* various defaults */ 8, vector<Vec4i>(), numeric_limits<int>::max(), 
						/* offset */ Point(-bounds.x, -bounds.y));
					return shared_ptr<Cluster>(new MatCluster(
						drawnContour, area, bounds, deepest, SMOOTHINGS[currSmoothing],
						false /* copy */, false /* this matrix is a part */));
				}
			}
		}
	}

	// we reach here if we reached the end of the image
	return shared_ptr<Cluster>();
}

OpenCV2BoxFilter ContourClusterer::makeBoxFilter(const cv::Mat_<unsigned char> &img)
{
	OpenCV2BoxFilter rval(SMOOTHINGS[NSMOOTHINGS-1]*2, CV_32S);
	rval.setNewImage(img);
	return rval;
}