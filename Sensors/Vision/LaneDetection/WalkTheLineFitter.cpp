#include "WalkTheLineFitter.h"
#include <iterator>
#include <algorithm>
#include "highgui.h"
#include <boost/random.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <assert.h>
#include <queue>

using namespace std;
using namespace cv;

#define WTLF WalkTheLineFitter // save some keystrokes

#define MAX_SMOOTH_KERNEL 5
#define ITERS 50
#define RADIUS_MEAN 60
#define RADIUS_VAR 100
#define CIRCUM_MAX_RUN_RADII 1.5
#define MIN_WALKED_POINTS 3

#define VISUALIZE

WTLF::WalkTheLineFitter(const cv::Mat_<float> &img)
{
	for (int smooth = 0; smooth < MAX_SMOOTH_KERNEL; ++smooth)
	{
		FilteredImgPtr sip(new FilteredImg(smooth, img));
		filtImgs.push_back(sip);
		// see shared_ptr best practices
	}
}

WTLF::FilteredImg::FilteredImg(int kernelSize, const cv::Mat_<float> &img)
: smoothingKernelSize(kernelSize), cumsum(img.rows*img.cols + 1, 0)
{
	if (smoothingKernelSize == 0)
		img.copyTo(filtImg);
	else
		cv::boxFilter(img, filtImg, -1 /* same depth as img */,
			Size(smoothingKernelSize, smoothingKernelSize), 
			Point(-1,-1) /* anchor at kernel center */,
			true /* normalize */);

	//calculate CDF
	cumsum[0] = 0.0f;
	Mat_<float>::const_iterator it = filtImg.begin(), 
		itEnd = filtImg.end();
	for (int i = 0; it != itEnd; ++it, ++i)
	{
		cumsum[i+1] = cumsum[i] + (*it)*(*it); /*pow(*it, 2)*/
	}
}

bool WTLF::Cluster::containsPoint(const cv::Point &pt) const
{
	//if (seeds.find(pt) != seeds.end()) return true;
	cv::Rect bounding(offset, mat.size());
	return bounding.contains(pt) && (mat(pt - offset) > 0);
}

struct WTLF::Cluster::FloodFillScratchpad
{
	cv::Mat_<float> img;
	cv::Mat_<unsigned char> marking;
	cv::Point br; // max bottom right found
#ifdef FLOOD_FILL_DFS
	int depth;
#else
	queue<cv::Point> q;
#endif
};

WTLF::ClusterPtr WTLF::FilteredImg::findCluster(const Point &pt)
{
	// check the cache
	vector<WTLF::ClusterPtr>::const_iterator it = clusterCache.begin(),
		itEnd = clusterCache.end();
	for (; it != itEnd; ++it)
	{
		WTLF::ClusterPtr cluster = *it;
		if (cluster->smoothingKernelSize == smoothingKernelSize && 
			cluster->containsPoint(pt))
		{
			cluster->seeds.push_back(pt); // will not insert a duplicate
			//cout << "Cluster cache hit" << endl;
			return cluster;
		}
	}

	WTLF::ClusterPtr cluster(new WTLF::Cluster(filtImg, pt));
	cluster->smoothingKernelSize = smoothingKernelSize;
	clusterCache.push_back(cluster);
	//cout << "Cluster cache miss" << endl;
	return cluster;
}

WTLF::Cluster::Cluster(const cv::Mat_<float> &img, cv::Point start)
{
	// actually find the cluster
	WTLF::Cluster::FloodFillScratchpad scratch;
	scratch.img = img;
	scratch.marking.create(img.size());
	scratch.marking.setTo(0);
	maxDepth = 0;
	deepest = start;
	offset = start;
	scratch.br = start;
#ifdef FLOOD_FILL_DFS
	scratch.depth = 0;
#endif

	// need to use BFS due to stack overflow!
	// note that the deepest is just the last point visited!
	scratch.q.push(start);
	while (!scratch.q.empty())
	{
		floodFill(scratch.q.front(), &scratch);
		scratch.q.pop();
	}

	mat = scratch.marking(
		Rect(offset, scratch.br));
	deepest -= offset;
}

void WTLF::Cluster::floodFill(
	cv::Point curr, 
	WTLF::Cluster::FloodFillScratchpad *scratch)
{
	if (scratch->img(curr) > 0 && scratch->marking(curr) == 0)
	{
		scratch->marking(curr) = 1;
		// make ROI include this point
		if (curr.x < offset.x) offset.x = curr.x;
		if (curr.y < offset.y) offset.y = curr.y;
		if (curr.x > scratch->br.x) scratch->br.x = curr.x;
		if (curr.y > scratch->br.y) scratch->br.y = curr.y;

#ifdef FLOOD_FILL_DFS
		// keep deepest point
		if (scratch->depth > maxDepth) {
			maxDepth = scratch->depth;
			deepest = curr;
		}

		// for the neighbors, we will recurse with higher depth
		scratch->depth++;
#else
		// in BFS the last point visited is deepest
		deepest = curr;
#endif
		// now handle neighbors
		const int dxs[] = {1, 0, -1, 0};
		const int dys[] = {0, 1, 0, -1};
		for (int i = 0; i < 4; ++i)
		{
			Point delta(dxs[i], dys[i]);
			Point next = curr + delta;
			if (next.x > 0 && next.y > 0 &&
				next.x < scratch->img.cols &&
				next.y < scratch->img.rows)
			{
#ifdef FLOOD_FILL_DFS
				floodFill(next, scratch);
#else
				scratch->q.push(next);
#endif
			}
		}

#ifdef FLOOD_FILL_DFS
		scratch->depth--;
#endif
	}
}

void WTLF::clusterDemoTrackbarCallback(int index, void *userData)
{
	WalkTheLineFitter *fitter = (WalkTheLineFitter*)userData;
	const char *windowName = "WalkTheLineFitter Cluster Demo";
	cv::imshow(windowName, fitter->filtImgs[index]->filtImg);
	fitter->clusterDemoTrackbarPos = index;
}

void WTLF::clusterDemoMouseCallback(int cvEvent, 
	int x, int y, int, void *userData)
{
	WalkTheLineFitter *fitter = (WalkTheLineFitter*)userData;
	const char *windowName = "WalkTheLineFitter Cluster Demo";
	if (cvEvent == CV_EVENT_LBUTTONDOWN)
	{
		int index = fitter->clusterDemoTrackbarPos;
		const Mat_<float> &img = 
			fitter->filtImgs[index]->filtImg;
		Point start(x, y);
		if (img(start) == 0) return; // clicked black
		WTLF::ClusterPtr cluster = 
			fitter->filtImgs[index]->findCluster(start);
		
		Mat_<Vec3b> rendered(img.size());
		rendered.setTo(0);
		for (int r = 0; r < rendered.rows; ++r)
		{
			for (int c = 0; c < rendered.cols; ++c)
			{
				if (img(r,c)) rendered(r,c) = Vec3b(255,255,255);
				if (cluster->containsPoint(Point(c,r)))
					rendered(r,c) = Vec3b(0,255,0);
			}
		}
		cv::circle(rendered, cluster->offset + cluster->deepest, 3, Scalar(0,255,0));

		cv::imshow(windowName, rendered);
	}
}

void WTLF::showClusterDemo()
{
	const char *windowName = "WalkTheLineFitter Cluster Demo";
	clusterDemoTrackbarCallback(0, this);
	cv::createTrackbar("FilteredImg #", windowName, 
		NULL, MAX_SMOOTH_KERNEL-1, 
		clusterDemoTrackbarCallback, this);
	cvSetMouseCallback(windowName, clusterDemoMouseCallback, this);
	cv::waitKey(0);
	cvDestroyWindow(windowName);
}

double WTLF::sampleRadius(double mean, double variance) {
	const double shape = ( mean*mean )/variance;
	double scale = variance/mean;

	static boost::mt19937 rng;
	boost::gamma_distribution<> gd( shape );
	boost::variate_generator<boost::mt19937&,boost::gamma_distribution<> >
		var_gamma( rng, gd );

	return scale*var_gamma();
}

int WTLF::Cluster::appendElementsOnCircle(
	const cv::Point &center,
	int radius,
	std::vector<WTLF::CirclePoint> &points) const
{
	int cx = center.x, cy = center.y;
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;

	vector<Point> offs;

	// x is 0, so handle top/bottom/left/right
	offs.push_back(Point(0,y));
	offs.push_back(Point(0,-y));
	offs.push_back(Point(y,0));
	offs.push_back(Point(-y,0));

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
			offs.push_back(Point(x,y));
			offs.push_back(Point(-x,y));
			offs.push_back(Point(x,-y));
			offs.push_back(Point(-x,-y));
		} else if (x < y) {
			offs.push_back(Point(x,y));
			offs.push_back(Point(-x,y));
			offs.push_back(Point(x,-y));
			offs.push_back(Point(-x,-y));
			offs.push_back(Point(y,x));
			offs.push_back(Point(-y,x));
			offs.push_back(Point(y,-x));
			offs.push_back(Point(-y,-x));
		}
	}

	vector<Point>::const_iterator it = offs.begin(),
		itEnd = offs.end();
	int count = 0;
	for (; it != itEnd; ++it)
	{
		Point pt = *it + center;
		if (pt.x >= 0 && pt.y >= 0 &&
			pt.x < mat.cols && pt.y < mat.rows)
		{
			WTLF::CirclePoint cpt;
			cpt.offCenter = *it;
			cpt.point = pt;
			cpt.value = mat(pt);
			points.push_back(cpt);
			++count;
		}
	}

	return count;
}

// sort into a circle, clockwise from top
bool WTLF::CirclePoint::operator < (
	const WTLF::CirclePoint &b)
{
	const WTLF::CirclePoint &a = *this;
	
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

int WTLF::Cluster::appendRunsOnCircle(
	const cv::Point &center, int radius, 
	std::vector<std::vector<WTLF::CirclePoint> > &runs) const
{
	vector<WTLF::CirclePoint> in;
	appendElementsOnCircle(center, radius, in);
	
	if (in.size() == 0) return 0;

	// sort the input
	std::sort(in.begin(), in.end());

	vector<WTLF::CirclePoint>::iterator it = in.begin(),
		itEnd = in.end();

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
				vector<WTLF::CirclePoint> vec(1, *it);
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
			vector<WTLF::CirclePoint> &firstRun = runs[firstIndex];
			vector<WTLF::CirclePoint> &lastRun = runs.back();
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

template<class T>
static inline T norm2(Point_<T> a, Point_<T> b)
{
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

int WTLF::appendAllFits(std::vector<Spline> &out)
{
	int outCount = 0;

	for (int iter = 0; iter < ITERS; ++iter)
	{
		// Sample a filter
		int iFilt = cv::theRNG().uniform(0, filtImgs.size());
		FilteredImgPtr filtImg = filtImgs[iFilt];
		
		// Sample a cluster start point
		int cumsumIndex;
		{
			float cumsumMin = filtImg->cumsum.front(); 
			float cumsumMax = filtImg->cumsum.back();
			float cumsumSample = cv::theRNG().uniform(cumsumMin, cumsumMax);
			// Binary search
			vector<float>::const_iterator cumsumIt = 
				lower_bound(filtImg->cumsum.begin(), 
					filtImg->cumsum.end(), cumsumSample);
			cumsumIndex = cumsumIt - filtImg->cumsum.begin();
		}
		Point clusterPoint(cumsumIndex % filtImg->filtImg.cols,
			cumsumIndex / filtImg->filtImg.cols);

		// Find the cluster and one end of it
		ClusterPtr cluster = filtImg->findCluster(clusterPoint);
		Point start = cluster->deepest;

		bool stopWalking = false;
		Point curr = start;
		vector<Point> walkedPoints;
		
		while (true)
		{
			walkedPoints.push_back(curr);

			// Sample a radius from the gamma distribution
			int radius = (int)sampleRadius(RADIUS_MEAN, RADIUS_VAR);

			vector<vector<CirclePoint> > runs;
			int runCount = cluster->appendRunsOnCircle(curr, radius, runs);
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

#ifdef VISUALIZE
			{
				Mat_<Vec3b> rendered(filtImg->filtImg.size());
				rendered.setTo(0);
				for (int r = 0; r < rendered.rows; ++r)
				{
					for (int c = 0; c < rendered.cols; ++c)
					{
						if (filtImg->filtImg(r,c)) rendered(r,c) = Vec3b(255,255,255);
						if (cluster->containsPoint(Point(c,r)))
							rendered(r,c) = Vec3b(0,255,0);
					}
				}
				for (int i = 0; i < walkedPoints.size(); ++i)
					cv::circle(rendered, walkedPoints[i]+cluster->offset, 3, Scalar(0,0,255), -1);
				for (int i = 0; i < runs.size(); ++i)
					for (int j = 0; j < runs[i].size(); ++j)
						rendered(runs[i][j].point+cluster->offset) = Vec3b(0,0,255-30*j);

				cv::imshow("WalkTheLineFitter debugging", rendered);
				cv::waitKey(50);
			}
#endif

			// first time, should see only one run of circle points, otherwise 2
			int expectedRuns = walkedPoints.size() == 1 ? 1 : 2;
			if (runCount != expectedRuns || maxRunLength > radius * CIRCUM_MAX_RUN_RADII){
				//cout << "Stopped after " << walkedPoints.size() << " walked points";
				//cout << " due to " << runCount << " runs and max runlength " << maxRunLength << endl;
				break;
			} else {
				// walk to the run furthest from the previous
				curr = runCenters[0];
				if (expectedRuns == 2)
				{
					Point prev = *(++walkedPoints.rbegin());
					if (norm2(prev,runCenters[0]) < norm2(prev,runCenters[1]))
						curr = runCenters[1];
				}
			}
		} // while walking

		// don't use very small walks
		if (walkedPoints.size() < MIN_WALKED_POINTS) continue;

		// create the spline
		Spline spline;
		spline.controlPoints.swap(walkedPoints);
		for (int i = 0; i < spline.controlPoints.size(); ++i)
			spline.controlPoints[i] += cluster->offset;
		out.push_back(spline);
		++outCount;

	} // for ITERS
	return outCount;
}