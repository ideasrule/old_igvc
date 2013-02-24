#include "RandomTestEnvironment.h"

#include "FloatIndexedArray.h"

#include <cstdlib>
#include <ctime>

#include <iostream> // debugging

namespace Pave_Libraries_Navigation {

//const int RandomTestEnvironment::X_MAX = 25;
//const int RandomTestEnvironment::Y_MAX = 25;
//const int RandomTestEnvironment::N_OBSTACLE = 5;
//const double RandomTestEnvironment::MAX_OBSTACLE_RADIUS = 2;
//const int RandomTestEnvironment::NUM_PATH_POINTS = 4;

const int RandomTestEnvironment::IMAGE_WIDTH = 600;
const int RandomTestEnvironment::IMAGE_HEIGHT = 600;

RandomTestEnvironment::RandomTestEnvironment(int x, int y, int nObs, double maxObsRadius, int numPathPts) :
	X_MAX(x), Y_MAX(y), N_OBSTACLE(nObs), MAX_OBSTACLE_RADIUS(maxObsRadius), NUM_PATH_POINTS(numPathPts),
    env(X_MAX, Y_MAX),
    gen((boost::uint32_t) time(0)),
    randomX(gen, boost::uniform_int<>(0, X_MAX - 1)),  // inclusive
    randomY(gen, boost::uniform_int<>(0, Y_MAX - 1)),  // inclusive
	randomObstacleRadius(gen, boost::uniform_real<>(0.0, MAX_OBSTACLE_RADIUS)),
	path(NUM_PATH_POINTS)
{
    env.setAll(FREE);

    // First, choose random path points and connect them
	GridSquare previousPoint = randomFreeGridSquare();
	path[0].push_back(previousPoint);
    for(int i = 1; i < numPathPoints(); i++) {
        GridSquare newPoint = randomFreeGridSquare();
        path[i].push_back(newPoint);
        drawLinePath(previousPoint, newPoint, path[i - 1]);
        previousPoint = newPoint;
    }

	typedef std::vector< std::vector<GridSquare> >::const_iterator VecIt;
	typedef std::vector<GridSquare>::const_iterator GsIt;
	for(VecIt vecIt = path.begin(); vecIt != path.end(); vecIt++) {
		for(GsIt gsIt = vecIt->begin(); gsIt != vecIt->end(); gsIt++) {
			setState(*gsIt, PATH);
		}
	}

	for(int i = 0; i < numObstacles(); i++) {
		typedef shared_ptr< std::vector<GridSquare> > VecPtr;
		VecPtr obstacle = randomObstacle();
		for(GsIt it = obstacle->begin(); it != obstacle->end(); it++) {
			setState(*it, OBSTACLE);
		}
	}
}

RandomTestEnvironment::~RandomTestEnvironment(void)
{
}

shared_ptr< std::vector<RandomTestEnvironment::Measurement> >
RandomTestEnvironment::getMeasurements(int segment, int index, Length range, Angle fov, Angle dfov)
{
	FloatIndexedArray<Angle> obstacles((int) (fov / dfov), -fov/2.0, fov/2.0);
	Angle *allAngles;
	int nObstacles = obstacles.cells(obstacles.minIndex(), obstacles.maxIndex(), allAngles);
	for(int i = 0; i < nObstacles; i++) {
		allAngles[i] = -1.0;
	}

	shared_ptr< std::vector<GridSquare> > fovSquares = getFovSquares(segment, index, range, fov);
	typedef std::vector<GridSquare>::const_iterator It;
	GridSquare curr = path[segment][index];
	Angle robotAngle = findRobotAngle(segment, index);

	for(It it = fovSquares->begin(); it != fovSquares->end(); it++) {
		GridSquare gs = *it;
		if(getState(gs) != OBSTACLE) { continue; }

		Angle gsAngle = angleDifference(findRobotAngleTo(segment, index, gs), robotAngle);
		Length gsDistance = distance(gs, curr);

        // find all intercepted heading angles
        Angle dtheta = atan(1.0/std::sqrt(2.0) / gsDistance);
        Angle *interceptedAngles;
        int nIntercepted = obstacles.cells(gsAngle - dtheta, gsAngle + dtheta, interceptedAngles);
        for(int i = 0; i < nIntercepted; i++) {
		    if(interceptedAngles[i] > gsDistance || interceptedAngles[i] < 0) {
			    interceptedAngles[i] = gsDistance;
		    }
        }
	}

	typedef shared_ptr< std::vector<RandomTestEnvironment::Measurement> > VecPtr;
	VecPtr measurements(new std::vector<Measurement>);

	Angle obsAngle = obstacles.minIndex();
	for(int i = 0; i < nObstacles; i++) {
		// distance to obstacle, minus imperfect correction for using
		// center of square instead of closest edge
		if(allAngles[i] > 0) {
			measurements->push_back(Measurement(
				allAngles[i] - 1.0/std::sqrt(2.0),
				obsAngle));
		}
		obsAngle += dfov;
	}

	return measurements;
}

void RandomTestEnvironment::getState(double& x, double& y, double& heading, int segment, int index)
{
    GridSquare current = path[segment][index];
    x = current.x;
    y = current.y;

    size_t N = path[segment].size();
    if(N > 1) {
        GridSquare segmentFirst = path[segment][0];
        GridSquare segmentLast = path[segment][N - 1];
        heading = wykobi::PI/2.0 - atan2(double(segmentLast.y - segmentFirst.y), double(segmentLast.x - segmentFirst.x));
    } else {
        heading = 0;
    }
}

RandomTestEnvironment::PixelColor RandomTestEnvironment::getColor(RandomTestEnvironment::CellState state)
{
    typedef RandomTestEnvironment::PixelColor PixelColor;
    switch(state) {
		// all bgr
        case OBSTACLE:
            return PixelColor(0xC7, 0x42, 0x24, 0); break; // dark blue
        case FREE:
            return PixelColor(255, 255, 255, 0); break;  // white
        case PATH:
            return PixelColor(0x23, 0xB8, 0x2D, 0); break;  // green
        case FOV:
            return PixelColor(0xE7, 0xCB, 0xF5, 0); // reddish
		default:
			return PixelColor(0, 0, 0, 0);
    }
}

cv::Mat_<cv::Vec3b> RandomTestEnvironment::copyToImage() const
{
    cv::Mat_<cv::Vec3b> image(IMAGE_WIDTH, IMAGE_HEIGHT);
    double pixelWidth = IMAGE_WIDTH / X_MAX;
    double pixelHeight = IMAGE_HEIGHT / X_MAX;

    for(int x = env.minX(); x < env.maxX(); x++) {
        for(int y = env.minY(); y < env.maxY(); y++) {
            cv::Range rows((int) (pixelWidth * x), (int) (pixelWidth * (x + 1))); 
            cv::Range cols((int) (pixelHeight * y), (int) (pixelHeight * (y + 1))); 
            image(rows, cols).setTo(getColor(getState(GridSquare(x, y))));
        }
    }

    return image;
}

cv::Mat_<cv::Vec3b> RandomTestEnvironment::copyToImageWithFov(int segment, int index, Length range, Angle fov) const
{
	cv::Mat_<cv::Vec3b> image = copyToImage();
    double pixelWidth = IMAGE_WIDTH / X_MAX;
    double pixelHeight = IMAGE_HEIGHT / X_MAX;

	shared_ptr< std::vector<GridSquare> > fovSquares = getFovSquares(segment, index, range, fov);
	typedef std::vector<GridSquare>::const_iterator It;

	for(It it = fovSquares->begin(); it != fovSquares->end(); it++) {
		GridSquare gs(*it);
		if(getState(gs) != PATH) {
            cv::Range rows((int) (pixelWidth * gs.x), (int) (pixelWidth * (gs.x + 1))); 
            cv::Range cols((int) (pixelHeight * gs.y), (int) (pixelHeight * (gs.y + 1))); 
            image(rows, cols).setTo(getColor(FOV));
		}
	}

	return image;
}

GridSquare RandomTestEnvironment::randomFreeGridSquare()
{
    GridSquare gs;
    do {
        gs = GridSquare(randomX(), randomY());
    } while(getState(gs) != FREE);

    return gs;
}

double RandomTestEnvironment::distance(GridSquare gs1, GridSquare gs2) const
{
	double dx = (double) (gs1.x - gs2.x);
	double dy = (double) (gs1.y - gs2.y);
	return std::sqrt(dx*dx + dy*dy);
}

shared_ptr< std::vector<GridSquare> > RandomTestEnvironment::randomObstacle()
{
	typedef shared_ptr<std::vector<GridSquare>> VecPtr;
	VecPtr vecPtr;
	do {
		double radius = randomObstacleRadius();
		int ceilRadius = (int) (radius + 1.0);
		GridSquare gs = randomFreeGridSquare();
		vecPtr = VecPtr(new std::vector<GridSquare>);

		int minX = std::max(gs.x - ceilRadius, env.minX());
		int maxX = std::min(gs.x + ceilRadius + 1, env.maxX()); // exclusive
		int minY = std::max(gs.y - ceilRadius, env.minY());
		int maxY = std::min(gs.y + ceilRadius + 1, env.maxY()); // exclusive

		for(int x = minX; x < maxX; x++) {
			for(int y = minY; y < maxY; y++) {
				GridSquare gstry(x, y);
				if(/*getState(gstry) != FREE*/ false) { // let obstacles overlap path for now
					vecPtr = VecPtr();
					goto randomObstacleContinue; // c++ has no labeled continue...
				} else if(distance(gs, gstry) < radius && getState(gstry) != PATH) {
					vecPtr->push_back(gstry);
				}
			}
		}
randomObstacleContinue: ;
	} while(!vecPtr);

	return vecPtr;
}

Angle RandomTestEnvironment::findRobotAngle(int segment, int index) const
{
	GridSquare from = path[segment][0];
	GridSquare to = path[segment][path[segment].size() - 1];
	double dx = (double) (to.x - from.x);
	double dy = (double) (to.y - from.y);
	return atan2(dy, dx);
}

Angle RandomTestEnvironment::findRobotAngleTo(int segment, int index, GridSquare gs) const
{
	GridSquare from = path[segment][index];
	GridSquare to = gs;
	double dx = (double) (to.x - from.x);
	double dy = (double) (to.y - from.y);
	return atan2(dy, dx);
}

Angle RandomTestEnvironment::angleDifference(Angle a1, Angle a2)
{
	Angle dAngle = abs(a1 - a2);
	if(dAngle > wykobi::PI) dAngle = 2*wykobi::PI - dAngle;
	return dAngle;
}

shared_ptr< std::vector<GridSquare> > RandomTestEnvironment::getFovSquares(int segment, int index, Length range, Angle fov) const
{
	using wykobi::PI;
	typedef shared_ptr< std::vector<GridSquare> > VecPtr;
	VecPtr fovSquares(new std::vector<GridSquare>);

	if(segment < 0 || segment >= (int) path.size() || fov > 2*PI
		|| index < 0 || index >= (int) path[segment].size() || (int) path[segment].size() < 2 ) {
		return fovSquares; // empty
	}

	GridSquare curr = path[segment][index];
	Angle robotAngle = findRobotAngle(segment, index);

	int ceilLength = (int) (range + 1.0);
	int minX = std::max(curr.x - ceilLength, env.minX());
	int maxX = std::min(curr.x + ceilLength + 1, env.maxX()); // exclusive
	int minY = std::max(curr.y - ceilLength, env.minY());
	int maxY = std::min(curr.y + ceilLength + 1, env.maxY()); // exclusive

	for(int x = minX; x < maxX; x++) {
		for(int y = minY; y < maxY; y++) {
			GridSquare gs(x, y);
			Angle dAngle = angleDifference(findRobotAngleTo(segment, index, gs), robotAngle);
			Length gsDistance = distance(gs, curr);

			if(dAngle <= fov / 2.0 && gsDistance < range) {
				fovSquares->push_back(gs);
			}
		}
	}

	return fovSquares;
}

void RandomTestEnvironment::drawLinePath(GridSquare start, GridSquare end, std::vector<GridSquare>& path)
{
	int x0 = start.x, y0 = start.y;
	int x1 = end.x, y1 = end.y;

	//Bresenham's Line Algorithm from Wikipedia
	int Dx = x1 - x0; 
	int Dy = y1 - y0;
	bool steep = (abs(Dy) >= abs(Dx));
	if (steep) {
        std::swap(x0, y0);
		std::swap(x1, y1);
		// recompute Dx, Dy after swap
		Dx = x1 - x0;
		Dy = y1 - y0;
	}
	int xstep = 1;
	if (Dx < 0) {
		xstep = -1;
		Dx = -Dx;
	}
	int ystep = 1;
	if (Dy < 0) {
		ystep = -1;		
		Dy = -Dy; 
	}
	int TwoDy = 2*Dy; 
	int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
	int E = TwoDy - Dx; //2*Dy - Dx
	int y = y0;
	int xDraw, yDraw;	

	for (int x = x0; x != x1; x += xstep) {
		if (steep) {
			xDraw = y;
			yDraw = x;
		} else {			
			xDraw = x;
			yDraw = y;
		}

		// plot
		path.push_back(GridSquare(xDraw, yDraw));

		// next
		if (E > 0) {
		    E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
		    y = y + ystep;
		} else {
		    E += TwoDy; //E += 2*Dy;
		}
	}
}

}
