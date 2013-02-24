#include "Cluster.h"

using namespace std;
using namespace cv;

Cluster::Cluster(int count,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing)
				 : boundsInWhole(boundsInWhole),
				 offset(boundsInWhole.tl()),
				 count(count),
				 deepest(deepest),
				 smoothing(smoothing)
{
}

Cluster::~Cluster() {}

MatCluster::MatCluster(const std::vector<cv::Point> &points,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing)
				 : Cluster(points.size(), boundsInWhole, deepest, smoothing),
				 mat(boundsInWhole.size())
{
	mat.setTo(0);
	for (vector<Point>::const_iterator it = points.begin(), itEnd = points.end();
		it != itEnd; ++it)
		mat(*it - offset) = 1;
}

MatCluster::MatCluster(const cv::Mat_<unsigned char> &wholeOrPart,
				 int count,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing, bool copy, bool matIsWhole)
				 : Cluster(count, boundsInWhole, deepest, smoothing)
{
	Mat_<unsigned char> part = matIsWhole ? wholeOrPart(boundsInWhole) : wholeOrPart;
	if (copy) part.copyTo(mat);
	else mat = part;
}

MatCluster::~MatCluster() {}

bool MatCluster::containsPoint(const cv::Point &pt) const
{
	if (!boundsInWhole.contains(pt)) return false;
	return mat(pt - offset) > 0;
}

void MatCluster::drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const
{
	/*
	Mat_<Vec3b> &part = out(boundsInWhole);
	Mat_<Vec3b>::iterator itOut = part.begin(), itOutEnd = part.end();
	Mat_<unsigned char>::const_iterator it = mat.begin(), itEnd = mat.end();
	for (; itOut != itOutEnd && it != itEnd; ++it, ++itOut)
	{
		if (*it > 0)
		{
			Vec3b &outPt = *itOut;
			outPt = color;
		}
	}
	*/
	out(boundsInWhole).setTo(Scalar(color[0], color[1], color[2]), mat);
}

// here, pointMat stores the points in its rows
FlannCluster::FlannCluster(const std::vector<cv::Point> &points,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing)
				 : Cluster(points.size(), boundsInWhole, deepest, smoothing)
{
	pointMat.create(points.size(), 2);
	for (int i = 0; i < points.size(); ++i)
	{
		pointMat[i][0] = (float)points[i].x;
		pointMat[i][1] = (float)points[i].y;
	}
	pointIndex.reset(new cv::flann::Index(
		pointMat, cv::flann::LinearIndexParams()));
}

FlannCluster::~FlannCluster() {}

bool FlannCluster::containsPoint(const cv::Point &pt) const
{
	if (!boundsInWhole.contains(pt)) return false;
	if (pointIndex->size() == 0) return false;
	vector<float> query(2,0);
	query[0] = pt.x; query[1] = pt.y;
	vector<int> indices(1,0);
	vector<float> dists(1,0);
	pointIndex->knnSearch(query, indices, dists, 
		1 /* neighbor */, 
		cv::flann::SearchParams(-1) /* exact neighbor */);
	return dists[0] < 1;
}

void FlannCluster::drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const
{
	for (int i = 0; i < pointMat.rows; ++i)
	{
		Point currPoint(pointMat[i][1], pointMat [i][2]);
		out(currPoint) = color;
	}
}

bool Cluster::PointCompare::operator() (const Point &a, const Point &b) const
{
	if (a.y < b.y) return true;
	else if (a.y == b.y) return a.x < b.x;
	else return false;
}

PointSetCluster::PointSetCluster(const std::vector<cv::Point> &points,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing)
				 : Cluster(points.size(), boundsInWhole, deepest, smoothing)
{
	pointSet.insert(points.begin(), points.end());
}

PointSetCluster::~PointSetCluster() {}

bool PointSetCluster::containsPoint(const cv::Point &pt) const
{
	if (!boundsInWhole.contains(pt)) return false;
	return pointSet.find(pt) != pointSet.end();
}

void PointSetCluster::drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const
{
	set<Point,PointCompare>::const_iterator it = pointSet.begin(), itEnd = pointSet.end();
	for (; it != itEnd; ++it)
		out(*it) = color;
}

ContourCluster::ContourCluster(const std::vector<cv::Point> &contourPoints,
				 int count,
				 const cv::Rect &boundsInWhole,
				 std::vector<cv::Point> deepest,
				 int smoothing)
				 : Cluster(count, boundsInWhole, deepest, smoothing),
				 contourMat(contourPoints, true /* copy data */)
{
}

ContourCluster::~ContourCluster() {}

bool ContourCluster::containsPoint(const cv::Point &pt) const
{
	if (!boundsInWhole.contains(pt)) return false;
	return pointPolygonTest(contourMat, pt, false) >= 0;
}

void ContourCluster::drawTo(cv::Mat_<cv::Vec3b> &out, cv::Vec3b color) const
{
	Scalar scalar(color[0], color[1], color[2]);
	vector<Point> contour = contourMat;
	vector<vector<Point> > contours(1, contour);
	drawContours(out, contours, 0, scalar, CV_FILLED);
}
