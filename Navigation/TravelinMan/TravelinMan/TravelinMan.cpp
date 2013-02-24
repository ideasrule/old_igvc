#include "Common.h"
#include "Geometry.h"
#include "GPSTransforms.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

using namespace Pave_Libraries_GPSTransforms;
using namespace Pave_Libraries_Geometry;

// How often to tick in s
#define INTERVAL .1
#define THRESHOLD .75

vector<point2d> waypoints;
SetWaypointsType swt;
int currentWaypoint = 0;
int numWaypoints;
StateEstimationType seUpdate;
PauseDisableType pdt;

void TimerHandler()
{
	if(currentWaypoint >= numWaypoints)
	{
		pdt.Disable = false;
		pdt.Pause = true;
		Messages::PauseDisable.publish(&pdt);
		return;
	}
	Messages::SetWaypoints.publish(&swt);
	if(sqrt(pow(seUpdate.Easting - swt.points[0].x, 2) + pow(seUpdate.Northing - swt.points[0].y, 2)) < THRESHOLD)
	{
		currentWaypoint++;
		if(currentWaypoint < numWaypoints)
		{
			swt.numPoints = numWaypoints - currentWaypoint;
			swt.points = &waypoints[currentWaypoint];
		}
	}
}

void StateEstimationHandler(void *data)
{
	StateEstimationType *se = (StateEstimationType*)data;
	seUpdate = *se;
}

void main(int argc, char **argv)
{
	// Read the waypoints
	bool success = false;
	char *missionFileName = Data::getString("Mission_File", success);
	if(!success)
		return;

	std::string line;
	std::ifstream missionFile(missionFileName);
	if(!missionFile.is_open())
		return;
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
		return;
	for(int i = 0; i < (int)latitudes.size(); i++)
	{
		CartesianLocalPoint p = GPSTransforms::ConvertToLocal(GeodeticPoint(latitudes[i], longitudes[i]));
		waypoints.push_back(point2d(p.X, p.Y));
	}

	swt.numPoints = (int)waypoints.size();
	numWaypoints = swt.numPoints;
	swt.points = &waypoints[0];

	seUpdate.Easting = 0;
	seUpdate.Northing = 0;

	IPCConnect("Mission Control");
	Timer::addTimer(&TimerHandler, (int)(INTERVAL * 1000));
	Messages::StateEstimation.subscribe(StateEstimationHandler, true, false);
	IPC_dispatch();
}

