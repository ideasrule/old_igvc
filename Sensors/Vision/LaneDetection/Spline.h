#ifndef SPLINE_H
#define SPLINE_H

#include <vector>
//#include <iterator>
#include "cv.h"
#include <assert.h>

template<typename T> class SplineIterator_;

template<typename T>
class Spline_ {
public:
	Spline_() 
		: controlPoints(), fitness(0.0), length(0.0), cardinalTension(0.0f) {}
    
    std::vector< cv::Point_<T> > controlPoints;
	double fitness, length;
	float cardinalTension;
    
    //cv::Point nearestPointTo(cv::Point point, bool linearApprox = true);

	// returns number of points drawn
	template<typename OutT>
    int drawTo(cv::Mat_<OutT> &mat, OutT value) const
	{
		/*
		float s = (1-cardinalTension)/((float)2);
		float cardinalArray[] = {
			(-1 * s), (2 - s), (s - 2), (s),
			(2 * s), (s - 3), (3 - (2 * s)), (-1 * s),
			(-1 * s), 0, s, 0,
			0, 1, 0, 0
		};
		Mat_<float> cardinal(4,4,cardinalArray); // assumes row-major
		
		// Duplicate first and last points so we assume starting tangents of 0;
		// this allows us to have all control points included in the curve.
		std::vector< cv::Point_<T> > ctrl;
		ctrl.reserve(controlPoints.size() + 2);
		ctrl.insert(ctrl.end(), controlPoints.front());
		ctrl.insert(ctrl.end(), controlPoints.begin(), controlPoints.end());
		ctrl.insert(ctrl.end(), controlPoints.back());
		
		int numPointsDrawn = 0;
		for (size_t i = 0; i <= ctrl.size() - 4; ++i)
		{
			float xArray[] = {
				ctrl[i].x, ctrl[i+1].x,
				ctrl[i+2].x, ctrl[i+3].x
			};
			float yArray[] = {
				ctrl[i].y, ctrl[i+1].y,
				ctrl[i+2].y, ctrl[i+3].y
			};
			Mat_<float> x(4,1,xArray), y(4,1,yArray);
			
			Mat_<float> cx = cardinal * x;
			Mat_<float> cy = cardinal * y;
			float *cxp = (float*)cx.data;
			float *cyp = (float*)cy.data;
			
			float increment = ((float)1)/500;
			int colout = -1, rowout = -1;
			for (float t = 0; t < 1; t += increment)
			{
				float xout = t*t*t*cxp[0] + t*t*cxp[1] + t*cxp[2] + cxp[3];
				float yout = t*t*t*cyp[0] + t*t*cyp[1] + t*cyp[2] + cyp[3];
				int nextcolout = xout;
				int nextrowout = yout;
				if ((nextcolout != colout || nextrowout != rowout) &&
					nextcolout >= 0 && nextcolout < mat.cols &&
					nextrowout >= 0 && nextrowout < mat.rows)
				{
					mat[nextrowout][nextcolout] = value;
					++numPointsDrawn;
				}
				rowout = nextrowout;
				colout = nextcolout;
			}
		}
		return numPointsDrawn;
		*/

		SplineIterator_<T> it(*this);
		int nPointsDrawn = 0;
		while (it.hasNextPoint())
		{
			Point_<T> p = it.nextPoint();
			int r = (int)p.y, c = (int)p.x;
			mat[r][c] = value;
			nPointsDrawn++;
		}
		return nPointsDrawn;
	}
};

typedef Spline_<int> Spline;

template<typename T>
class SplineIterator_ {
private:
	float increment;

	cv::Matx44f cardinal;
	std::vector< cv::Point_<T> > ctrl;
	int i;
	float t;
	cv::Matx14f cx, cy;
	T xout, yout;

	void init(const Spline_<T> &sp)
	{
		increment = ((float)1)/500;
		i = -1;
		t = 1;

		float s = (1-sp.cardinalTension)/((float)2);
		cardinal = Matx44f(
			(-1 * s), (2 - s), (s - 2), (s),
			(2 * s), (s - 3), (3 - (2 * s)), (-1 * s),
			(-1 * s), 0, s, 0,
			0, 1, 0, 0
		);
		
		// Duplicate first and last points so we assume starting tangents of 0;
		// this allows us to have all control points included in the curve.
		ctrl.reserve(sp.controlPoints.size() + 2);
		ctrl.insert(ctrl.end(), sp.controlPoints.front());
		ctrl.insert(ctrl.end(), sp.controlPoints.begin(), sp.controlPoints.end());
		ctrl.insert(ctrl.end(), sp.controlPoints.back());
	}

public:

	SplineIterator_(const Spline_<T> &sp)
	{
		init(sp);
	}

	bool hasNextPoint() const
	{
		// at end of iteration, t >= 1, and i will be ctrl.size() - 4 or greater
		return t < 1 || i < ((int)ctrl.size() - 4);
	}

	cv::Point_<T> nextPoint()
	{
		T xoutnext, youtnext;
		do
		{
			if (t >= 1 && i < ((int)ctrl.size() - 4))
			{
				++i;
				//assert(i >= 0 && i <= ((int)ctrl.size() - 4));

				cv::Matx14f x(
					ctrl[i].x, ctrl[i+1].x,
					ctrl[i+2].x, ctrl[i+3].x);
				cv::Matx14f y(
					ctrl[i].y, ctrl[i+1].y,
					ctrl[i+2].y, ctrl[i+3].y);
				
				for (int r = 0; r < 4; ++r)
				{
					cx(r) = x.dot(cardinal.row(r));
					cy(r) = y.dot(cardinal.row(r));
				}

				t = 0;
			}

			cv::Matx14f trow(t*t*t, t*t, t, 1);
			xoutnext = trow.dot(cx);
			youtnext = trow.dot(cy);

			t += increment;

		} while (xoutnext == xout && youtnext == yout);

		xout = xoutnext;
		yout = youtnext;
		return Point_<T>(xout, yout);
	}
};

#endif //SPLINE_H
