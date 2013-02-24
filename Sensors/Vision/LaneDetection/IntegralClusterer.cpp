#include "IntegralClusterer.h"
#include <queue>
#include <assert.h>

using namespace cv;
using namespace std;

#define MIN_CLUSTER_SIZE 200

static const int SMOOTHINGS[] = {
	0,5,10
};
#define NSMOOTHINGS 3

const int IntegralClusterer::num_smoothings = NSMOOTHINGS;

IntegralClusterer::IntegralClusterer(const Mat_<float> &img, 
									 const Mat_<int> &sum, int singleSmoothing)
: Clusterer(img), sum(sum), curr(0,0), 
currSmoothing(singleSmoothing < 0 ? 0 : singleSmoothing), 
restrictCurrSmoothing(singleSmoothing >= 0),
markings(NSMOOTHINGS)
{
	floodFillQueue.set_capacity(img.rows*img.cols);
}

IntegralClusterer::~IntegralClusterer()
{
}

shared_ptr<Cluster> IntegralClusterer::emitCluster()
{
	while (curr.y < img.rows && currSmoothing < NSMOOTHINGS)
	{
		vector<Point> clusterPoints;
		Rect bounds(curr.x, curr.y, 1, 1);
		Point deepest = curr;
		
		int smoothFactor = SMOOTHINGS[currSmoothing];
		Mat_<unsigned char> &markingAtCurrSmoothing = markings[currSmoothing];
		if (markingAtCurrSmoothing.empty())
		{
			markingAtCurrSmoothing.create(img.size());
			markingAtCurrSmoothing.setTo(0);
		}
		
		// Flood fill starting at curr, if it hasn't already been marked?
		if (!markingAtCurrSmoothing(curr))
		{
			//queue<Point> pointQueue; 
			// over 1/3 of performance was the automatic destruction
			//pointQueue.clear();
			//pointQueue.push_back(curr);
			//assert(pointQueue.empty());

			//assert(floodFillQueue.empty());
			floodFillQueue.push_back(curr);
			while (!floodFillQueue.empty())
			{
				Point pt = floodFillQueue.front();
				floodFillQueue.pop_front();
				if (!markingAtCurrSmoothing(pt) &&
					sumHasPoint(pt.x,pt.y,smoothFactor))
				{
					markingAtCurrSmoothing(pt) = 1;
					clusterPoints.push_back(pt);
					deepest = pt;
					bounds |= Rect(pt.x,pt.y,1,1);

					// insert neighbors
					const int dxs[] = {1, 0, -1, 0};
					const int dys[] = {0, 1, 0, -1};
					for (int i = 0; i < 4; ++i)
					{
						Point delta(dxs[i], dys[i]);
						Point next = pt + delta;
						//assert(!floodFillQueue.full());
						if (next.x >= 0 && next.y >= 0 &&
							next.x < img.cols && next.y < img.rows)
						{
							floodFillQueue.push_back(next);
						}
					}
				}
			}
		}
		
		// Move to next point & (possibly) smoothing regardless of
		// whether we've found something
		if (restrictCurrSmoothing) ++curr.x;
		else
		{
			++currSmoothing;
			if (currSmoothing >= NSMOOTHINGS)
			{
				currSmoothing = 0;
				++curr.x;
			}
		}
		if (curr.x >= img.cols)
		{
			curr.x = 0;
			++curr.y;
		}

		// If we've found something sizeable
		if (clusterPoints.size() >= MIN_CLUSTER_SIZE)
		{
			return shared_ptr<Cluster>(new MatCluster(
				clusterPoints, bounds, vector<Point>(1,deepest), smoothFactor));
		}
	}

	// we reach here if we reached the end of the image
	return shared_ptr<Cluster>();
}