#include "ipc.h"
#include "Common.h"
#include "GPSTransforms.h"
#include "wykobi.hpp"
#include <fstream>
#include <cmath>

//#define COORD_FROM_GPS
//#define ADD_BOUNDARY

using namespace Pave_Libraries_GPSTransforms;
using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Geometry;

using std::ifstream;
using std::ofstream;

const double HALF_BARREL_SIZE = 0.5;
char * const COSTMAP_PATH = "C:\\IGVC\\Navigation\\Costmaps\\default.txt";
const double X_UNIT_SIZE = 0.1;
const double Y_UNIT_SIZE = 0.1;

int numWaypoints;
int currentWaypoint = 0;

vector<point2d> waypoints;
vector<double> boundaryXs;
vector<double> boundaryYs;

bool receivedState = false;
StateEstimationType set;
SetWaypointsType swt;
vector<double> oldBarrelXs, oldBarrelYs;

void TimerHandler()
{
	CostMapType cmt;
	cmt.xMin = -200;
	cmt.xMax = 200;
	cmt.yMin = -200;
	cmt.yMax = 200;
	cmt.xUnitSize = X_UNIT_SIZE;
	cmt.yUnitSize = Y_UNIT_SIZE;

	vector<double> barrelXs, barrelYs;
	vector<double> deletedXs, deletedYs;

#ifndef COORD_FROM_GPS
	//waypoints.push_back(point2d(71,71));
	numWaypoints = (int)waypoints.size();
#endif

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
		for(double x = barrelXs[i] - HALF_BARREL_SIZE; x <= barrelXs[i] + HALF_BARREL_SIZE; x += cmt.xUnitSize)
			for(double y = barrelYs[i] - HALF_BARREL_SIZE; y <= barrelYs[i] + HALF_BARREL_SIZE; y += cmt.yUnitSize)
			{
				xs.push_back(x);
				ys.push_back(y);
			}
	}
		

	newBarrelPoints = static_cast<int>(xs.size());

	for(int i = 0; i < numDeletedBarrels; i++) {     // deleted barrels
		for(double x = deletedXs[i] - HALF_BARREL_SIZE; x <= deletedXs[i] + HALF_BARREL_SIZE; x += cmt.xUnitSize)
			for(double y = deletedYs[i] - HALF_BARREL_SIZE; y <= deletedYs[i] + HALF_BARREL_SIZE; y += cmt.yUnitSize)
			{
				xs.push_back(x);
				ys.push_back(y);
			}
	}

	totalBarrelPoints = static_cast<int>(xs.size());



//	for (int i = 0; i < boundaryXs.size(); ++i) {
//		xs.push_back(boundaryXs[i]);
//		ys.push_back(boundaryYs[i]);
//	}

	double *costs = new double[xs.size()];
	for(int i = 0; i < newBarrelPoints; i++)
		costs[i] = 0.1;
	for(int i = newBarrelPoints; i < totalBarrelPoints; i++)
		costs[i] = 0;
//	for(int i = totalBarrelPoints; i < xs.size(); i++)
//		costs[i] = 0.1;


	cmt.numPoints = static_cast<int>(xs.size());
	if(cmt.numPoints > 0)
	{
		cmt.xUpdates = &xs[0];
		cmt.yUpdates = &ys[0];
		cmt.costUpdates = costs;
	}
	Messages::CostMap.publish(&cmt);
	delete costs;

	bool waitForNewPath = false;
	if(currentWaypoint < numWaypoints && sqrt(pow(waypoints[currentWaypoint].x - set.Easting, 2) + pow(waypoints[currentWaypoint].y - set.Northing, 2)) < .5) {
		waitForNewPath = true;
		currentWaypoint++;	
	}

	swt.numPoints = numWaypoints - currentWaypoint;
	swt.points = &waypoints[currentWaypoint];
	Messages::SetWaypoints.publish(&swt);


	PauseDisableType pd;
	pd.Pause = false;
	if(waitForNewPath)
		pd.Pause = true;
	if (currentWaypoint >= numWaypoints) {
		currentWaypoint = 0;
	}
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

#ifdef COORD_FROM_GPS
	bool success = false;
	char *missionFileName = Data::getString("Mission_File", success);
	if(!success)
		return -1;
 
	std::string line;
	std::ifstream missionFile(missionFileName);
	if(!missionFile.is_open())
		return -1;
	vector<double> latitudes, longitudes;
	while(!missionFile.eof())
	{
		getline(missionFile, line);
		latitudes.push_back(atof(line.c_str()));
		if(missionFile.eof())
		{
			latitudes.pop_back();
			break;
		}
		getline(missionFile, line);
		longitudes.push_back(atof(line.c_str()));
	}

	if(latitudes.size() == 0)
		return -1;
	for(int i = 0; i < (int)latitudes.size(); i++)
	{
		CartesianLocalPoint p = GPSTransforms::ConvertToLocal(GeodeticPoint(latitudes[i], longitudes[i]));
		waypoints.push_back(point2d(p.X, p.Y));
	}

	swt.numPoints = (int)waypoints.size();
	numWaypoints = swt.numPoints;
	swt.points = &waypoints[0];

#endif

#ifdef ADD_BOUNDARY
	bool successful = false;
	char *boundaryFileName = Data::getString("Boundary_File", successful);
	if(successful)
	{
		vector<double> latitudes, longitudes;
		std::string line;
		std::ifstream boundaryFile(boundaryFileName);
		if(!boundaryFile.is_open())
			return 0;
		while(!boundaryFile.eof())
		{
			std::getline(boundaryFile, line);
			latitudes.push_back(atof(line.c_str()));
			if(boundaryFile.eof())
			{
				latitudes.pop_back();
				break;
			}
			getline(boundaryFile, line);
			longitudes.push_back(atof(line.c_str()));
		}
		for(int i = 0; i < (int)latitudes.size(); i++)
		{
			CartesianLocalPoint p = GPSTransforms::ConvertToLocal(GeodeticPoint(latitudes[i], longitudes[i]));
			latitudes[i] = p.X;
			longitudes[i] = p.Y;
		}
		for(int i = 0; i < (int)latitudes.size() - 1; i++)
		{
			double distance = wykobi::distance(latitudes[i], longitudes[i], latitudes[i+1], longitudes[i+1]);
			double numCells = distance / X_UNIT_SIZE;
			int numCellsI = (int)numCells;
			for(int j = 0; j <= numCellsI; j++)
			{
				double xCoord = latitudes[i] + (latitudes[i+1] - latitudes[i]) * (j / numCells);
				double yCoord = longitudes[i] + (longitudes[i+1] - longitudes[i]) * (j / numCells);
				boundaryXs.push_back(xCoord);
				boundaryYs.push_back(yCoord);
			}
		}
		if(latitudes.size() > 2)
		{
			double distance = wykobi::distance(latitudes[latitudes.size() - 1], longitudes[latitudes.size() - 1], latitudes[0], longitudes[0]);
			double numCells = distance / X_UNIT_SIZE;
			int numCellsI = (int)numCells;
			for(int j = 0; j <= numCellsI; j++)
			{
				double xCoord = latitudes[latitudes.size() - 1] + (latitudes[0] - latitudes[latitudes.size() - 1]) * (j / numCells);
				double yCoord = longitudes[latitudes.size() - 1] + (longitudes[0] - longitudes[latitudes.size() - 1]) * (j / numCells);
				boundaryXs.push_back(xCoord);
				boundaryYs.push_back(yCoord);
			}
		}
	}
#endif



//	seUpdate.Easting = 0;
//	seUpdate.Northing = 0;

	IPCConnect("NavTester");
	
	Messages::StateEstimation.subscribe(StateEstimationHandler, true, false);
	Timer::addTimer(TimerHandler, 1000);
	IPC_dispatch();
}
