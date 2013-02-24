#include "OvershootClusterer.h"
#include "highgui.h"
#include <sstream>

using namespace std;
using namespace cv;

#define CLUSTERER_OVERSHOOTS 9
#define MIN_CLUSTER_SIZE 200

OvershootClusterer::OvershootClusterer(const cv::Mat_<float> &img)
: Clusterer(img), smallestOvershootVisit(img.size(), CLUSTERER_OVERSHOOTS),
currRow(0), currCol(0), debugWindowName(NULL), debugX(-1), debugY(-1),
temps(CLUSTERER_OVERSHOOTS)
{
	for (int i = 0; i < CLUSTERER_OVERSHOOTS; ++i)
	{
		//temps[i].marking.create(img.size());
		temps[i].emitsClusters = (i == 0 || !(i&(i-1)));
		// only emit clusters if 0,1,2,4,8,...
	}
}

OvershootClusterer::~OvershootClusterer() {}

shared_ptr<Cluster> OvershootClusterer::emitCluster()
{
	// the clusters from the previous pixel
	while (recentClusters.empty() && currRow < img.rows)
	{
		Point curr(currCol, currRow);
		if (img(curr) > 0)
		{
			// clear all temporaries
			for (int i = 0; i < CLUSTERER_OVERSHOOTS; ++i)
			{
				Temporary &temp = temps[i];
				//temp.marking.create(img.size());
				//temp.marking = 0; // horrible performance, 3/4 of the samples!
				//if (!temp.edge.empty()) temp.edge.clear();
				if (!temp.points.empty()) temp.points.clear();
				temp.bounds = Rect(curr, Size(1,1));
				temp.deepest = curr;
				temp.count = 0;
			}

			// do the multi-resolution flood fill
			FloodFillArgs start;
			start.pt = curr;
			start.overshoot = 0; 
			start.maxOvershoot = 0;
			temps[start.maxOvershoot].floodFillQueue.push(start);
			
			for (int i = 0; i < CLUSTERER_OVERSHOOTS; ++i)
			{
				Temporary &temp = temps[i];
				while (!temp.floodFillQueue.empty())
				{
					floodFill(temp.floodFillQueue.front());
					temp.floodFillQueue.pop();
					shared_ptr<Cluster> c(new MatCluster(
						temp.points, temp.bounds, vector<Point>(1,temp.deepest), i));
					debugCluster = c;
					//renderDebug();
					//waitKey(1);
				}
			}

			// now investigate whether any clusters were created!
			for (int i = 0; i < CLUSTERER_OVERSHOOTS; ++i)
			{
				Temporary &temp = temps[i];
				if (temp.emitsClusters && temp.count >= MIN_CLUSTER_SIZE)
				{
					// note that this will swap an empty set with temp.edge
					shared_ptr<Cluster> c(new MatCluster(
						temp.points, temp.bounds, vector<Point>(1,temp.deepest), i));
					recentClusters.push(c);
				}
			}
		}

		// Move to the next pixel
		++currCol;
		if (currCol >= img.cols)
		{
			currCol = 0;
			++currRow;
		}
	}

	// shouldn't be a reference here since it'd be invalid once popped
	if (recentClusters.empty())
	{
		showDebugWindow(NULL); // turn it off if it's on
		return shared_ptr<Cluster>();
	}
	shared_ptr<Cluster> c = recentClusters.front();
	recentClusters.pop();
	debugCluster = c;
	renderDebug();
	return c;
}

// NOTE: args should not be a reference, it gets modified
void OvershootClusterer::floodFill(FloodFillArgs args)
{
	if (!args.pt.inside(Rect(Point(0,0),img.size()))) return;

	// Keep track of how much we've overshot,
	// which is how many pixels we've been treading water
	// since we've seen dry land, so to speak
	if (img(args.pt) <= 0) ++args.overshoot;
	else args.overshoot = 0;

	// Stop if we've overshot too much
	if (args.overshoot >= CLUSTERER_OVERSHOOTS) return;

	// And keep track of the maximum overshoot since the start
	// Invariant: maxOvershoot < CLUSTERER_OVERSHOOTS
	if (args.overshoot > args.maxOvershoot)
		args.maxOvershoot = args.overshoot;

	// Stop if we've already visited this 
	// at the max overshoot or lower, since
	// if we've already seen a small cluster, we've already
	// seen its super-clusters!
	// Note that this also keeps us from infinite-looping
	// onto points already visited from this start
	if (smallestOvershootVisit(args.pt) <= args.maxOvershoot) return;

	// So this was smaller than the previous overshoot visit!
	smallestOvershootVisit(args.pt) = args.maxOvershoot;

	// Fill the temporaries at the current overshoot or higher
	for (int i = args.maxOvershoot; i < CLUSTERER_OVERSHOOTS; ++i)
	{
		Temporary &temp = temps[i];
		if (temp.emitsClusters)
		{
			Rect addToBounds(args.pt.x, args.pt.y, 1, 1);
			temp.bounds |= addToBounds;
			
			++temp.count;
			temp.points.push_back(args.pt);
			temp.deepest = args.pt;
		}
	}

	// now handle neighbors
	const int dxs[] = {1, 0, -1, 0};
	const int dys[] = {0, 1, 0, -1};
	for (int i = 0; i < 4; ++i)
	{
		Point delta(dxs[i], dys[i]);
		Point next = args.pt + delta;
		/*if (next.x >= 0 && next.y >= 0 &&
			next.x < img.cols &&
			next.y < img.rows)*/
		{
			FloodFillArgs newArgs = args;
			newArgs.pt = next;
			temps[args.maxOvershoot].floodFillQueue.push(newArgs);
		}
	}
}

void OvershootClusterer::drawTo(cv::Mat_<cv::Vec3b> &out) const
{
	out.setTo(0);
	out.setTo(Scalar(255,255,255), img > 0);

	Mat_<Vec3b>::iterator itOut = out.begin(), itOutEnd = out.end();
	Mat_<unsigned char>::const_iterator it = smallestOvershootVisit.begin(), 
		itEnd = smallestOvershootVisit.end();
	for (; itOut != itOutEnd && it != itEnd; ++it, ++itOut)
	{
		if ((*it) < CLUSTERER_OVERSHOOTS)
		{
			unsigned char nonRed = (*it)*(255/CLUSTERER_OVERSHOOTS);
			*itOut = Vec3b(nonRed,nonRed,255);
		}
	}
}

void OvershootClusterer::showDebugWindow(const char *windowName)
{
	if (!windowName && debugWindowName)
	{
		cvDestroyWindow(debugWindowName);
		debugWindowName = NULL;
	}
	else if (windowName && (!debugWindowName ||
		strcmp(windowName, debugWindowName) != 0))
	{
		debugWindowName = windowName;
		renderDebug();
		cvSetMouseCallback(debugWindowName, 
			debugWindowMouseCallback, this);
	}
}

void OvershootClusterer::debugWindowMouseCallback(int cvEvent,
	int x, int y, int, void *userData)
{
	OvershootClusterer *clusterer = (OvershootClusterer*)userData;
	if (cvEvent == CV_EVENT_MOUSEMOVE)
	{
		clusterer->debugX = x;
		clusterer->debugY = y;
		clusterer->renderDebug();
	}
}

void OvershootClusterer::renderDebug() const
{
	if (!debugWindowName) return;

	Mat_<Vec3b> rendered(img.size());
	rendered.setTo(0);
	drawTo(rendered);
	if (debugCluster)
	{
		debugCluster->drawTo(rendered, Vec3b(0,255,0));
		cv::circle(rendered, debugCluster->deepest[0], 5, Scalar(255,0,0));
	}
	if (debugX >= 0 && debugY >= 0)
	{
		int fontFace = FONT_HERSHEY_SIMPLEX;
		double fontScale = 0.5;
		int thickness = 1;

		stringstream ss;
		ss << "(" << debugX << "," << debugY << "): ";\
		if (debugCluster && 
			debugCluster->containsPoint(Point(debugX, debugY)))
			ss << "containsPoint ";
		
		ss << "sov " << (int)smallestOvershootVisit(Point(debugX, debugY)) << " ";

		int baseline = 0;
		Size textSize = getTextSize(ss.str(), 
			fontFace, fontScale, thickness, &baseline);
		putText(rendered, ss.str(), Point(0, img.rows - textSize.height - baseline), 
			fontFace, fontScale, Scalar(255,255,0), thickness);
	}
	imshow(debugWindowName, rendered);
}