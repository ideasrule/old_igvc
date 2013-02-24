#include "SplineFitter.h"
#include "cv.h"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cmath>
#include <boost/random.hpp>
#include <boost/random/gamma_distribution.hpp>
//#include <random>

#define VISUALIZE

#include "highgui.h"

using namespace cv;
using namespace std;
using namespace Pave_Libraries_Camera;
using namespace boost;

static Mat_<float> expDecayKernel;
static mt19937 rng;

// From http://stackoverflow.com/questions/1719070/what-is-the-right-approach-when-using-stl-container-for-median-calculation
///Return the median of a sequence of numbers defined by the random
///access iterators begin and end.  The sequence must not be empty
///(median is undefined for an empty set).
///
///The numbers must be convertible to double.
template<class T>
static inline T median(vector<T> &vec)
{
	size_t size = vec.size();
	if (size == 0) return (T)0;
	size_t middleIdx = size/2;
	vector<T>::iterator begin = vec.begin(),
		end = vec.end(), 
		target = begin + middleIdx;
	nth_element(begin, target, end);

	if(size % 2 != 0) { // Odd number of elements
		return *target;
	} else { //Even number of elements
		T a = *target;
		vector<T>::iterator targetNeighbor = target-1;
		nth_element(begin, targetNeighbor, end);
		return (a+*targetNeighbor)/2.0;
	}
}

//http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html
static void draw8WaySymm(int cx, int cy, int x, int y, vector<Point> &v)
{
	if (x == 0) {
		v.push_back(Point(cx, cy + y));
		v.push_back(Point(cx, cy - y));
		v.push_back(Point(cx + y, cy));
		v.push_back(Point(cx - y, cy));
	} else if (x == y) {
		v.push_back(Point(cx + x, cy + y));
		v.push_back(Point(cx - x, cy + y));
		v.push_back(Point(cx + x, cy - y));
		v.push_back(Point(cx - x, cy - y));
	} else if (x < y) {
		v.push_back(Point(cx + x, cy + y));
		v.push_back(Point(cx - x, cy + y));
		v.push_back(Point(cx + x, cy - y));
		v.push_back(Point(cx - x, cy - y));
		v.push_back(Point(cx + y, cy + x));
		v.push_back(Point(cx - y, cy + x));
		v.push_back(Point(cx + y, cy - x));
		v.push_back(Point(cx - y, cy - x));
	}
}

//http://www.cs.unc.edu/~mcmillan/comp136/Lecture7/circle.html
static vector<Point> drawCircle(Point center, int radius)
{
	vector<Point> v;
	v.reserve((int)(2*3.5*radius));

	int cx = center.x, cy = center.y;
    int x = 0;
    int y = radius;
    int p = (5 - radius*4)/4;

    draw8WaySymm(center.x, center.y, x, y, v);
    while (x < y) {
        x++;
        if (p < 0) {
            p += 2*x+1;
        } else {
            y--;
            p += 2*(x-y)+1;
        }
        draw8WaySymm(center.x, center.y, x, y, v);
    }

	return v;
}

template<class T>
static inline T norm2(Point_<T> a, Point_<T> b)
{
	return (a.x-b.x)*(a.x-b.x) + (a.y-b.y)*(a.y-b.y);
}

static double rgamma(double mean, double variance) {
  const double shape = ( mean*mean )/variance;
  double scale = variance/mean;

  gamma_distribution<> gd( shape );
  variate_generator<mt19937&,gamma_distribution<> > var_gamma( rng, gd );

  return scale*var_gamma();
}

template<class T>
class SplineControlOrdering
{
	Point_<T> p0;
public:
	SplineControlOrdering(Point_<T> p) : p0(p) {}
	bool operator() (Point_<T> p1, Point_<T> p2)
	{ return norm2<T>(p0,p1) < norm2<T>(p0,p2); }
	//{ return p1.y < p2.y; }
};

SplineFitter::SplineFitter(const Mat_<float> &m)
	: img(m.size()), imgPts(), splineImg(m.size()), multImg(m.size()),
	cumsum(m.rows * m.cols + 1, 0), 
	numPoints(6), nIterations(300)
{
	if (expDecayKernel.empty())
	{
		enum {KERNEL_SIZE = 25, CENTER_OFFSET = 12};
		expDecayKernel.create(KERNEL_SIZE, KERNEL_SIZE);
		for (int r = 0; r < KERNEL_SIZE; ++r)
		{
			for (int c = 0; c < KERNEL_SIZE; ++c)
			{
				int dr = abs(r - CENTER_OFFSET);
				int dc = abs(c - CENTER_OFFSET);
				expDecayKernel(r, c) = (double)(exp(-0.02*(double)(dr*dr+dc*dc)));
			}
		}
		normalize(expDecayKernel, expDecayKernel, 1, 0, NORM_L1);
		/*
		Mat_<Vec3b> cKernel(KERNEL_SIZE, KERNEL_SIZE);
		vector<Mat> planes;
		split(cKernel, planes);
		expDecayKernel.convertTo(planes[0], planes[0].type(), 256);
		expDecayKernel.convertTo(planes[1], planes[1].type(), 256);
		expDecayKernel.convertTo(planes[2], planes[2].type(), 256);
		merge(planes, cKernel);
		*/
#ifdef VISUALIZE
		cv::imshow("Kernel", expDecayKernel);
#endif
	}

	
	m.copyTo(img);
	GaussianBlur(img, img, Size(7,7), 0);
	/*
	cv::Ptr<cv::FilterEngine> expDecay = 
		cv::createLinearFilter(img.type(), img.type(), expDecayKernel);
	expDecay->apply(img, img);
	*/
	for (int r = 0; r < img.rows; ++r)
	{
		for (int c = 0; c < img.cols; ++c)
		{
			if (img[r][c] > 0) 
				imgPts.insert(imgPts.end(), Point(c,r));
		}
	}
#ifdef VISUALIZE
	//cv::imshow("Spline fitter image end of ctor", img);
	//cv::waitKey();
#endif VISUALIZE
}

void SplineFitter::displayImgPts(const char *windowName) const
{
#ifdef VISUALIZE
	Mat_<float> m(img.rows, img.cols);
	list<Point>::const_iterator 
		imgPtsIt = imgPts.begin(),
		imgPtsItEnd = imgPts.end();

	// For every point in the image, check if it's in the spline image
	for (; imgPtsIt != imgPtsItEnd; ++imgPtsIt)
	{
		const Point &pt = *imgPtsIt;
		m(pt) = img(pt);
	}

	cv::imshow(windowName, m);
#endif
}

Point SplineFitter::sampleAtRadius(Point center, float radius, Point nw) const
{
	/*Point nwOnRadius;
	if (nw != center)
	{
		Point vec = nw - center;
		nwOnRadius = center + vec*(radius/(float)cv::norm(vec));
	}
	float nwR2 = radius * radius;*/
	Point nwVec = nw - center;

	vector<Point> points = drawCircle(center, (int)radius);
	vector<Point> filt;
	filt.reserve(points.size());
	Rect bounding(Point(0,0), img.size());
	for (vector<Point>::const_iterator it = points.begin(); it != points.end(); ++it)
	{
		Point p = *it;
		if (p.inside(bounding) && img(p) > 0)
		{
			if (nw == center) filt.push_back(p); // sentinel (null-valued) nw given
			else //if (nwVec.dot(p-center) < 0) // obtuse angle with nw
			{
				Point pVec = p - center;
				float pLen = sqrt((float)norm2(pVec,Point()));
				Point2f pUnit = Point2f(pVec.x/pLen, pVec.y/pLen);
				float nwLen = sqrt((float)norm2(nwVec,Point()));
				Point2f nwUnit = Point2f(nwVec.x/nwLen, nwVec.y/nwLen);
				float cosTheta = pUnit.dot(nwUnit);
				if (cosTheta < -0.7f) // want a very obtuse angle
					filt.push_back(p);
			}
		}
	}

	if (filt.empty()) return center;

	int i = cv::theRNG().uniform(0, filt.size());
	//vector<Point>::const_iterator it = filt.begin();
	//advance(it, i);
	//return *it;
	return filt[i];
}

Point SplineFitter::sample() const
{
	int i = cv::theRNG().uniform(0, imgPts.size());
	list<Point>::const_iterator it = imgPts.begin();
	advance(it, i);
	return *it;

	/*
    float minRand = cumsum.front(), maxRand = cumsum.back();
    
    //Pick a random value in the range of cumsum
	//float r = ((float)rand() / (RAND_MAX + 1.0));
	//float sample = minRand + r * (maxRand - minRand);
	float sample = cv::theRNG().uniform(minRand, maxRand);

    //Find where sample lives in the distribution
    int minIndex = 0, maxIndex = cumsum.size();
	while(maxIndex - minIndex > 1)
	{
		int testIndex = (maxIndex + minIndex) / 2;
		float testVal = cumsum[testIndex];
		if(testVal <= sample)
			minIndex = testIndex;
		else
			maxIndex = testIndex;
	}
	int ncols = img.cols;
    return Point(minIndex % ncols, minIndex / ncols);
	*/
}

Spline SplineFitter::sampleSpline() const
{
	const double RADIUS_MEAN = 50, RADIUS_VAR = 100;
	Spline s;
	s.controlPoints.reserve(numPoints);

	Point p0 = sample();
	s.controlPoints.push_back(p0);

	Point p1;
	float r01 = (float)rgamma(RADIUS_MEAN, RADIUS_VAR);
	p1 = sampleAtRadius(p0, r01, p0);
	if (p1 == p0) return s;
	s.controlPoints.push_back(p1);

	while (s.controlPoints.size() < numPoints) {
		float r = (float)rgamma(RADIUS_MEAN, RADIUS_VAR);
		Point center, prev;
		{
			vector<Point>::const_reverse_iterator rit = s.controlPoints.rbegin();
			center = *rit;
			++rit;
			prev = *rit;
		}
		Point p = sampleAtRadius(center, r, prev);
		if (p == center) break;
		else {
			s.controlPoints.push_back(p);
		}
	}
/*
	for (int i = 0; i < numPoints; ++i)
		s.controlPoints[i] = sample();
	Point first = s.controlPoints.front();
	SplineControlOrdering<int> ordering(first);
	std::stable_sort(s.controlPoints.begin(), s.controlPoints.end(), ordering);
*/
	return s;
}

//Calculate cumulative distributition function (for sampling)
//This must be done before each fitting, since
//previous fittings may have zeroed out some of the image
void SplineFitter::updateCumsum()
{
	cumsum[0] = 0.0f;
	Mat_<float>::const_iterator it = img.begin(), itEnd = img.end();
	for (int i = 0; it != itEnd; ++it, ++i)
	{
		cumsum[i+1] = cumsum[i] + pow(*it, 2);
	}
}

Spline SplineFitter::fitOne(bool zeroOut)
{
	//updateCumsum();

    Spline best;
	for (int iter = 0; iter < nIterations; iter++)
	{
		//if (iter % 10 == 0) cout << iter << endl;
		// Render a random spline with control points
		// selected as the RANSAC random sample
		Spline spline = sampleSpline();
		//splineImg.setTo(0);
		//int pointsDrawn = spline.drawTo(splineImg, 1.0f);
		//boxFilter(splineImg, splineImg, -1 /* same depth */, Size(5,5), 
		//	Point(-1,-1), false /* don't normalize*/);

		//GaussianBlur(splineImg, splineImg, Size(5,5), 0);
		
/*
		// Overlay the images
		multiply(img, splineImg, multImg);
		
		// Calculate spline fitness
		spline.fitness = 0;
		Mat_<float>::const_iterator it = multImg.begin(), 
			itEnd = multImg.end();
		for ( ; it != itEnd; ++it)
			spline.fitness += (*it);
		// Do we want to normalize by the spline length (pointsDrawn)?
*/

		/*
		list<Point>::iterator 
			imgPtsIt = imgPts.begin(),
			imgPtsItEnd = imgPts.end();

		// For every point in the image, check if it's in the spline image
		for (; imgPtsIt != imgPtsItEnd; ++imgPtsIt)
		{
			Point &pt = *imgPtsIt;
			float val = splineImg(pt);
			spline.fitness += val*val; // * img(pt)
		}
		spline.fitness /= pointsDrawn;
		*/

		SplineIterator_<int> it(spline);
		int nPointsDrawn = 0;
		Rect imgBounds(0,0,640,480);
		bool first = true;
		Point old;
		while (it.hasNextPoint())
		{
			Point pt = it.nextPoint();
			
			if (pt.inside(imgBounds))
			{
				//if (!first) spline.length += norm2(pt, old);
				first = false;

				float val = img(pt);
				spline.fitness += val*val;
				spline.length += 1;
			}
			old = pt;
		}

		spline.fitness /= spline.length;

		// Skip consensus for now, just choose the best match
		if (spline.fitness > best.fitness)
			best = spline;
	}

#ifdef VISUALIZE
	
	{
		Mat_<Vec3b> visual(img.rows, img.cols);
		img.convertTo(visual, visual.type());
		best.drawTo(visual, Vec3b(256,0,0));
		imshow("Best Spline   Overlay", visual);
	}
	
#endif
	
	if (zeroOut)
	{
		// Subtract the drawing of the best spline from the image
		splineImg.setTo(0.0f);
		best.drawTo(splineImg, 1.0f);
		boxFilter(splineImg, splineImg, -1 /* same depth */, Size(30,30), 
			Point(-1,-1), false /*don't normalize*/);
		subtract(img, splineImg, img);
		
		//img.setTo(Scalar(0), splineImg); // DOESN'T WORK?

		list<Point>::iterator 
			imgPtsIt = imgPts.begin(),
			imgPtsItEnd = imgPts.end();

		// For every point in the image, check if it's in the spline image
		while (imgPtsIt != imgPtsItEnd)
		{
			Point &pt = *imgPtsIt;
			if (splineImg(pt) > 0)
				imgPtsIt = imgPts.erase(imgPtsIt);
			else ++imgPtsIt;
		}

		// Also erase from the img
		
#ifdef VISUALIZE
		displayImgPts("After zero-out");
#endif

	}

	cout << "Found spline" << endl;
    
    return best;
}

Spline_<float> SplineFitter::transformToGround(Spline s, shared_ptr<Frame> frm)
{
	Spline_<float> groundSpline;
	groundSpline.controlPoints.assign(s.controlPoints.size(),Point_<float>());
	
	for (int iCtrl = 0; iCtrl < s.controlPoints.size(); ++iCtrl)
	{
		vector<float> localGroundX, localGroundY;
		Point &p = s.controlPoints[iCtrl];
		int radius = GROUND_TRANSFORM_PIXEL_RADIUS;
		do
		{
			for (int row = p.y - radius;
				row < p.y + radius; ++row)
			{
				for (int col = p.x - radius;
					col < p.x + radius; ++col)
				{
					if (row >= 0 && row < frm->height &&
						col >= 0 && col < frm->width &&
						frm->validArr[row][col] && 
						!frm->obstacle[row][col])
					{
						Point3f &groundPoint = frm->transformedCloud(p);
						localGroundX.push_back(groundPoint.x);
						localGroundY.push_back(groundPoint.y);
					}
				}
			}
			radius *= 2;
		} while (localGroundX.size() == 0 && radius < frm->width);

		float medx = median(localGroundX);
		float medy = median(localGroundY);
		groundSpline.controlPoints[iCtrl] = Point(medx, medy);
	}

	groundSpline.fitness = s.fitness;
	groundSpline.cardinalTension = s.cardinalTension;
	
	return groundSpline;
}
