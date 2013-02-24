#include "RandomTestEnvironment.h"
using namespace Pave_Libraries_Navigation;

#include "IPCMessage.h"
#include "IPCConnect.h"
#include "ipc.h"

#include "Messages.h"
using namespace Pave_Libraries_Common;

#include "cv.h"
#include "highgui.h"

#include <iostream>
using namespace std;

const int X_MAX = 200;
const int Y_MAX = 200;
const int N_OBSTACLE = 15;
const double MAX_OBSTACLE_RADIUS = 20;
const int NUM_PATH_POINTS = 100;
const double FOV = wykobi::PI/2; // rad
const double D_FOV = 0.0001; // rad
const double RANGE = 50.0;
const double SCALE_FACTOR = 1.0;  // actual occupancy map range is SCALE_FACTOR*RANGE, for example

struct Tester {
	Tester() : s(0), i(0), env(X_MAX, Y_MAX, N_OBSTACLE, MAX_OBSTACLE_RADIUS, NUM_PATH_POINTS),
		path(env.getPath())
	{
	}

	typedef std::vector<RandomTestEnvironment::Measurement> MeasurementVec;
	typedef shared_ptr<MeasurementVec> VecPtr;

    void copyMeasurementsToVisionUpdate(const MeasurementVec& measurements, VisionUpdateType& update) {
        update.numPoints = measurements.size();
        update.xVals = new double[update.numPoints];
        update.yVals = new double[update.numPoints];
        update.costs = new double[update.numPoints];

        typedef MeasurementVec::const_iterator It;
        int i = 0;
        for(It it = measurements.begin(); it != measurements.end(); it++, i++) {
            update.xVals[i] = it->distance / SCALE_FACTOR * sin(it->angle);
            update.yVals[i] = it->distance / SCALE_FACTOR * cos(it->angle);
            update.costs[i] = 1.0;
        }
    }

	void step() {
		if((size_t) s >= path.size() || (size_t) i >= path[s].size()) {
            cv::waitKey(1);
            return;
		}

        StateEstimationType state;
        env.getState(state.Easting, state.Northing, state.Heading, s, i);
        Messages::StateEstimation.publish(&state);

		VecPtr measurements = env.getMeasurements(s, i, RANGE, FOV, D_FOV);
		cout << measurements->size() << endl;
        VisionUpdateType update;
        //update.numLanePoints = update.numLanes = 0;
		update.nsplines = 0;
		std::vector<SplineIPC> splineVec;
		// make a fake spline a meter to the left and right of the bot
		float fakeXControls[] = {-1, -1, 1, 1};
		float fakeYControls[] = {1, 2};
		for (int i = 0; i <= 2; i += 2)
		{
			SplineIPC spline;
			spline.numcontrols = 2;
			spline.xcontrols = &fakeXControls[i];
			spline.ycontrols = fakeYControls;
			splineVec.push_back(spline); // shallow copy
		}
		update.nsplines = splineVec.size();
		update.all_splines = &splineVec[0]; // will be persistent until published
        update.state = state;

        env.getState(update.state.Easting, update.state.Northing, update.state.Heading, s, i);
        copyMeasurementsToVisionUpdate(*measurements, update);
        Messages::VisionUpdate.publish(&update);

		cv::imshow("Random Environment", env.copyToImageWithFov(s, i, RANGE, FOV));
		cv::waitKey(1);

		i++;
		if(i == (int) path[s].size()) {
			i = 0;
			s++;
		}
	}

	int s, i;
	RandomTestEnvironment env;
	const std::vector< std::vector<GridSquare> > path;
};

static Tester tester;

void stepTester(void *, unsigned long, unsigned long) {
	tester.step();
}

int main(void)
{
	Pave_Libraries_Common::IPCConnect("OccupancyMapFunctionalTester");
    IPC_addPeriodicTimer(100, stepTester, 0);

    cv::imshow("Random Environment", tester.env.copyToImage());
    cv::waitKey(1);

    IPC_dispatch();
}