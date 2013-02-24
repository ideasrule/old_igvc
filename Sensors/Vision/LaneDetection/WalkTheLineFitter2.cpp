#include "WalkTheLineFitter2.h"
#include "highgui.h"
#include <list>
#include <assert.h>
#include <iostream>
#include <iomanip>
#include "PerfTimer.h"
#include <boost/circular_buffer.hpp>

using namespace std;
using namespace cv;
using namespace boost;

#define WTLF WalkTheLineFitter2

#define CIRCUM_MAX_RUN_RADII 0.7
#define MIN_WALKED_POINTS 3

static const int RADII[] = {80, 50};
#define NRADII 2

template<class T>
static inline T norm2(Point_<T> a, Point_<T> b)
{
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

static int visualizationNumber = 0;

std::map<const int, shared_ptr<std::vector<cv::Point> > > WTLF::offsetCache;

WTLF::WalkTheLineFitter2(const Mat_<float> &img)
: img(img)
{
}

void WTLF::processCluster(const shared_ptr<Cluster> &cluster, VisualizationLevel visualize)
{
	for (int iDeep = 0; iDeep < cluster->deepest.size(); ++iDeep)
	{
		bool stopWalking = false;
		Point curr = cluster->deepest[iDeep];
		InternalLane lane;
		lane.redundant = false;

		while (!stopWalking)
		{
			lane.ctrl.push_back(curr);
			int expectedRuns = lane.ctrl.size() == 1 ? 1 : 2;

			bool foundNewCurr = false;

			for(int iRadius = 0; 
				!stopWalking && !foundNewCurr && iRadius < NRADII; 
				++iRadius)
			{
				int radius = RADII[iRadius];

				vector<CirclePoint> points;
				int pointCount = appendClusterCircle(
					cluster, curr, radius, points);
				assert(pointCount == points.size());

				if (visualize == CIRCLE_POINTS)
				{
					Mat_<Vec3b> rendered(img.size(), 0);
					cluster->drawTo(rendered, Vec3b(100,100,100));
					// Draw the points in the order they arrive
					for (int i = 0; i < pointCount; ++i)
					{
						if (points[i].point.inside(Rect(Point(),img.size())))
						{
							rendered(points[i].point) = points[i].value ? Vec3b(0,255,0) : Vec3b(0,0,255);
							cv::imshow("Circle Drawing Visualization", rendered);
							cv::waitKey(1);
						}
					}
				}

				vector<vector<CirclePoint> > runs;
				int runCount = sortCirclePointsToRuns(
					points, runs);

				assert(runCount == runs.size());
				
				vector<size_t> runLengths;
				size_t maxRunLength = 0;
				vector<Point> runCenters;

				for (int i = 0; i < runs.size(); i++)
				{
					runLengths.push_back(runs[i].size());
					if (runs[i].size() > maxRunLength)
						maxRunLength = runs[i].size();
					runCenters.push_back(runs[i][runs[i].size()/2].point);
				}

				if (visualize > NONE)
				{
					Mat_<Vec3b> rendered(img.size(), 0);
					rendered.setTo(Scalar(255, 255, 255), img > 0);
					cluster->drawTo(rendered, Vec3b(0,255,0));

					for (int i = 0; i < lane.ctrl.size(); ++i)
						cv::circle(rendered, lane.ctrl[i], 3, Scalar(0,0,255), -1);
					for (int i = 0; i < runs.size(); ++i)
						for (int j = 0; j < runs[i].size(); ++j)
							rendered(runs[i][j].point) = Vec3b(0,0,255-30*j);

					cv::imshow("WalkTheLineFitter visualization", rendered);
					if (visualize == CLUSTER_SAVE)
					{
						std::stringstream filename;
						filename << "wtlf" << std::setfill('0') << std::setw(5) << (visualizationNumber++) << ".png";
						cv::imwrite(filename.str(), rendered);
					}

					// Uncomment this to pause on very "tall" lanes for debugging
					//cv::waitKey(cluster->count > 100 ? 0 : 10);
					cv::waitKey(10);
				} // visualize

				if (maxRunLength > radius * CIRCUM_MAX_RUN_RADII + cluster->smoothing)
				{
					// Stop immediately if a run was too long, 
					// since there's too much noise to walk reliably,
					// or this is an obstacle or the sky that's not a run!
					stopWalking = true;
				}
				else if (runCount != expectedRuns)
				{
					// first time, should see only one run of circle points, otherwise 2
					// but this should not cause us to totally stop the walk,
					// instead try a different radius in case
					// there was a hole in the lane!
				}
				else
				{
					// walk to the run furthest from the previous
					if (expectedRuns < 2)
					{
						curr = runCenters[0];
						foundNewCurr = true;
					}
					else 
					{
						Point &prev = lane.ctrl[lane.ctrl.size() - 2];
						if (norm2(prev,runCenters[0]) > norm2(prev,runCenters[1]) &&
							norm2(prev,runCenters[0]) > norm2(prev,curr))
						{
							curr = runCenters[0];
							foundNewCurr = true;
						}
						else if (norm2(prev,runCenters[1]) > norm2(prev,runCenters[0]) &&
							norm2(prev,runCenters[1]) > norm2(prev,curr))
						{
							curr = runCenters[1];
							foundNewCurr = true;
						}
					}
				}
			} // radii

			if (!foundNewCurr)
				stopWalking = true;
		} // while walking

		if (lane.ctrl.size() >= MIN_WALKED_POINTS)
			lanes.push_back(lane);
	} // starting at different deepest points
}

void WTLF::filterRedundantLanes()
{
	Mat_<float> firstLastPointMat(2*lanes.size(), 2);
	for (int i = 0; i < lanes.size(); ++i)
	{
		float *row = firstLastPointMat[i*2];
		row[0] = lanes[i].ctrl.front().x;
		row[1] = lanes[i].ctrl.front().y;
		row[2] = lanes[i].ctrl.back().x;
		row[3] = lanes[i].ctrl.back().y;
	}

	cv::flann::Index_<float> firstLastIndex(
		firstLastPointMat,
		cv::flann::AutotunedIndexParams());
}

int WTLF::appendLanes(vector<vector<Point> > &out) const
{
	int count = 0;
	vector<InternalLane>::const_iterator it = lanes.begin(),
		itEnd = lanes.end();
	for (; it != itEnd; ++it)
	{
		if (!it->redundant)
		{
			out.push_back(it->ctrl);
			++count;
		}
	}
	return count;
}

int WTLF::appendSplines(vector<Spline> &out) const
{
	int count = 0;
	vector<InternalLane>::const_iterator it = lanes.begin(),
		itEnd = lanes.end();
	for (; it != itEnd; ++it)
	{
		if (!it->redundant)
		{
			Spline s;
			s.controlPoints = it->ctrl;
			out.push_back(s);
			++count;
		}
	}
	return count;
}

int WTLF::appendClusterCircle(
	const shared_ptr<Cluster> &cluster,
	const cv::Point &center, int radius, 
	std::vector< CirclePoint > &points) const
{
	//PerfTimer method("appendClusterCircle");
	
	map<const int, shared_ptr<vector<Point> > >::const_iterator
		cacheHit = offsetCache.find(radius);
	if (cacheHit != offsetCache.end())
	{
		const shared_ptr<vector<Point> > &entry = cacheHit->second;
		points.reserve(points.size() + entry->size());
		for (vector<Point>::iterator it = entry->begin(), itEnd = entry->end();
			it != itEnd; ++it)
		{
			Point pt = *it + center;
			//if (cluster->boundsInWhole.contains(pt))
			{
				CirclePoint cpt;
				cpt.offCenter = *it;
				cpt.point = pt;
				cpt.value = cluster->containsPoint(pt);
				points.push_back(cpt);
			}
		}
		return entry->size();
	}

	// Draw the circle
	int cx = center.x, cy = center.y;
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;

	enum Octant {
		OCT_SSE = 0, OCT_ESE,
		OCT_ENE, OCT_NNE,
		OCT_NNW, OCT_WNW,
		OCT_WSW, OCT_SSW, NUM_OCTS};
	circular_buffer<cv::Point> offs[NUM_OCTS]; // the magic bullet!
	for (int i = 0; i < NUM_OCTS; ++i) offs[i].set_capacity(4*radius); 
	// to be conservative

	// x is 0, so handle top/bottom/left/right
	offs[OCT_SSE].push_back(Point(0,y));
	offs[OCT_NNW].push_back(Point(0,-y));
	offs[OCT_ENE].push_back(Point(y,0));
	offs[OCT_WSW].push_back(Point(-y,0));
	int totalOffs = 4;

	while (x < y) {
		// draw 8-way symmetry
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }

		if (x == y) {
			offs[OCT_SSE].push_back(Point(x,y));
			offs[OCT_ENE].push_back(Point(-x,y));
			offs[OCT_WSW].push_back(Point(x,-y));
			offs[OCT_NNW].push_back(Point(-x,-y));
			totalOffs += 4;
			// and this will terminate
		} else if (x < y) {
			offs[OCT_SSE].push_back(Point(x,y));
			offs[OCT_SSW].push_front(Point(-x,y));
			offs[OCT_NNE].push_front(Point(x,-y));
			offs[OCT_NNW].push_back(Point(-x,-y));
			offs[OCT_ESE].push_front(Point(y,x));
			offs[OCT_WSW].push_back(Point(-y,x));
			offs[OCT_ENE].push_back(Point(y,-x));
			offs[OCT_WNW].push_front(Point(-y,-x));
			totalOffs += 8;
		}
	}

	points.reserve(totalOffs + points.size());
	shared_ptr<vector<Point> > offCacheEntry(new vector<Point>());
	offCacheEntry->reserve(totalOffs);

	int iCache = 0;
	for (int oct = 0; oct < NUM_OCTS; ++oct)
	{
		circular_buffer<Point>::const_iterator it = offs[oct].begin(),
			itEnd = offs[oct].end();
		for (; it != itEnd; ++it)
		{
			Point pt = *it + center;
			//if (cluster->boundsInWhole.contains(pt))
			{
				CirclePoint cpt;
				cpt.offCenter = *it;
				cpt.point = pt;
				cpt.value = cluster->containsPoint(pt);
				points.push_back(cpt);
				offCacheEntry->push_back(*it);
			}
		}
	}

	const map<const int, shared_ptr<vector<Point> > >::value_type 
		cachePair(radius, offCacheEntry);
	offsetCache.insert(cachePair);
	return totalOffs;
}

// sort into a circle, clockwise from top
bool WTLF::CirclePoint::operator < (
	const CirclePoint &b)
{
	const CirclePoint &a = *this;
	
	bool rval;
	// first see if they're on opposite sides
	if (a.offCenter.x >= 0 && b.offCenter.x < 0)
		rval = true; // a on right, b on left
	else if (a.offCenter.x < 0 && b.offCenter.x >= 0)
		rval = false; // a on left, b on right
	else { // both on same side
		float aslope = a.offCenter.y/(float)a.offCenter.x;
		float bslope = b.offCenter.y/(float)b.offCenter.x;
		rval = aslope > bslope;
	}
	return rval;
}

int WTLF::sortCirclePointsToRuns(
	std::vector<CirclePoint> &points,
	std::vector< std::vector<CirclePoint> > &runs) const
{
	//PerfTimer method("sortCirclePointsToRuns");
	
	if (points.size() == 0) return 0;

	// sort the input
	{
		//PerfTimer sorting("actual sorting");
		//std::sort(points.begin(), points.end());
	}

	vector<CirclePoint>::iterator it = points.begin(),
		itEnd = points.end();

	int firstIndex = runs.size();
	bool firstStartedRun = (it->value > 0);
	bool lastWasRun = false;
	int runCount = 0;
	for (; it != itEnd; ++it)
	{
		if (!it->value) {
			lastWasRun = false;
		} else {
			if (!lastWasRun) {
				vector<CirclePoint> vec(1, *it);
				runs.push_back(vec);
				lastWasRun = true;
				++runCount;
			} else {
				runs.back().push_back(*it);
			}
		}
	}

	// handle connecting the two ends
	// push the combo onto the front
	if (firstStartedRun && lastWasRun)
	{
		{
			vector<CirclePoint> &firstRun = runs[firstIndex];
			vector<CirclePoint> &lastRun = runs.back();
			firstRun.insert(
				firstRun.end(),
				lastRun.begin(),
				lastRun.end());
		}
		runs.pop_back();
		--runCount;
	}

	return runCount;
}
