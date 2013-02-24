#include "ipc.h"
#include "Common.h"
#include <fstream>
#include <cmath>

using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Geometry;
using std::ifstream;
using std::ofstream;

char * const COSTMAP_PATH = "C:\\IGVC\\Navigation\\Costmaps\\default.txt";

//int numBarrels = 12;
//double barrelXs[] = {8, 11, 13, 11, 8, 6, 4, 2, 0, 10, 12, 14};
//double barrelYs[] = {10,13,13, 15, 5, 5, 5,  5, 5, 5, 5, 5};

int numWaypoints = 3;
int currentWaypoint = 0;
point2d waypoints[] = {point2d(15.04, 12.22), point2d(10.13, 18.92), point2d(-.42, -6.47)};
bool receivedState = false;
StateEstimationType set;
//bool firstTime = true;
vector<double> oldBarrelXs, oldBarrelYs;

void TimerHandler()
{
	CostMapType cmt;
	cmt.xMin = -200;
	cmt.xMax = 200;
	cmt.yMin = -200;
	cmt.yMax = 200;
	cmt.xUnitSize = .1;
	cmt.yUnitSize = .1;

	vector<double> barrelXs, barrelYs;
	vector<double> deletedXs, deletedYs;

	ifstream inFile;
	do {
		inFile.open(COSTMAP_PATH, ios::in);
	} while (!inFile);



	double inValue;
	int numBarrels;
	int numDeletedBarrels;

	while (inFile >> inValue) {
		barrelXs.push_back(inValue);
		inFile >> inValue;
		barrelYs.push_back(inValue);
	}
	inFile.close();
	numBarrels = static_cast<int>(barrelXs.size());
	
	int oldSize = static_cast<int>(oldBarrelXs.size());
	if (oldSize != numBarrels) {
		for (int i = 0; i < oldSize; i++) {
			int j;
			for (j = 0; j < numBarrels; j++) {
				if (fabs(oldBarrelXs[i] - barrelXs[j]) < 0.0000001 && fabs(oldBarrelYs[i] - barrelYs[j]) < 0.0000001)
					break;
			}
			if (j == numBarrels) {        // obstacles that existed previously but no more
				deletedXs.push_back(oldBarrelXs[i]);
				deletedYs.push_back(oldBarrelYs[i]);
			}
		}
	}
	oldBarrelXs = barrelXs;
	oldBarrelYs = barrelYs;
	numDeletedBarrels = static_cast<int>(deletedXs.size());

	vector<double> xs;
	vector<double> ys;
	int newBarrelPoints;
	int totalBarrelPoints;
	for(int i = 0; i < numBarrels; i++) {      // barrels
		for(double x = barrelXs[i] - .5; x <= barrelXs[i] + .5; x += cmt.xUnitSize)
			for(double y = barrelYs[i] - .5; y <= barrelYs[i] + .5; y += cmt.yUnitSize)
			{
				xs.push_back(x);
				ys.push_back(y);
			}
	}
	newBarrelPoints = static_cast<int>(xs.size());

	for(int i = 0; i < numDeletedBarrels; i++) {     // deleted barrels
		for(double x = deletedXs[i] - .5; x <= deletedXs[i] + .5; x += cmt.xUnitSize)
			for(double y = deletedYs[i] - .5; y <= deletedYs[i] + .5; y += cmt.yUnitSize)
			{
				xs.push_back(x);
				ys.push_back(y);
			}
	}
	totalBarrelPoints = static_cast<int>(xs.size());

	cmt.numPoints = static_cast<int>(xs.size());
	double *costs = new double[xs.size()];
	for(int i = 0; i < newBarrelPoints; i++)
		costs[i] = 50;
	for(int i = newBarrelPoints; i < totalBarrelPoints; i++)
		costs[i] = 0;
	if(cmt.numPoints > 0)
	{
		cmt.xUpdates = &xs[0];
		cmt.yUpdates = &ys[0];
		cmt.costUpdates = costs;
	}
	Messages::CostMap.publish(&cmt);
	delete costs;

	//if(currentWaypoint < numWaypoints && sqrt(pow(waypoints[currentWaypoint].x - set.Easting, 2) + pow(waypoints[currentWaypoint].y - set.Northing, 2)) < .5)
	//	currentWaypoint++;

	//SetWaypointsType swt;
	//swt.numPoints = numWaypoints - currentWaypoint;
	//swt.points = waypoints + currentWaypoint;
	//Messages::SetWaypoints.publish(&swt);

	PauseDisableType pd;
	pd.Pause = false;
	if(currentWaypoint >= numWaypoints)
		pd.Pause = true;
	pd.Disable = false;
	Messages::PauseDisable.publish(&pd);

	/*StateEstimationType set;
	set.Northing = 0;
	set.Easting = 0;
	set.Heading = 0;
	Messages::StateEstimation.publish(&set);*/
}

void StateEstimationHandler(void *data)
{
	receivedState = true;
	set = *((StateEstimationType*)data);
}

int main(int argc, char **argv)
{
	IPC_connectModule("NavTester", "localhost");
	

	Messages::StateEstimation.subscribe(StateEstimationHandler, true, false);
	Timer::addTimer(TimerHandler, 300);
	//Timer::addTimer(TimerHandler, 1000);
	IPC_dispatch();
}
