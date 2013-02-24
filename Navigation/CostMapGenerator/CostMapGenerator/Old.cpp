// CostMapGenerator.cpp : Defines the entry point for the console application.
//
#define NUM_POINTS 1000
#include "ipc.h"
#include "Common.h"
using namespace Pave_Libraries_Common;

double getRand()
{
	return (rand() % 10001) / 10000.0;
}

//int main(int argc, char** argv)
//{
//	IPC_connectModule("CostMapGenerator", "localhost");
//	double xUpdates[NUM_POINTS];
//	double yUpdates[NUM_POINTS];
//	double costUpdates[NUM_POINTS];
//	CostMapType output;
//	output.xMin = -100;
//	output.xMax = 100;
//	output.xUnitSize = .1;
//	output.yMin = -100;
//	output.yMax = 100;
//	output.yUnitSize = .1;
//	output.numPoints = NUM_POINTS;
//	for(int i = 0; i < NUM_POINTS; i++)
//	{
//		costUpdates[i] = 50;
//		if(i < NUM_POINTS / 4)
//		{
//			yUpdates[i] = 3;
//			xUpdates[i] = 3 + 20 * (i / (NUM_POINTS / 4.0));
//		}
//		else if(i < NUM_POINTS / 2)
//		{
//			yUpdates[i] = 3;
//			xUpdates[i] = -20 * ((i - NUM_POINTS / 4.0) / (NUM_POINTS / 4.0));
//		}
//		else if(i < 3 * NUM_POINTS / 4)
//		{
//			yUpdates[i] = -3;
//			xUpdates[i] = 20 * ((i - NUM_POINTS / 2.0) / (NUM_POINTS / 4.0));
//		}
//		else
//		{
//			yUpdates[i] = -3;
//			xUpdates[i] = -20 * (i - 3.0 * NUM_POINTS / 4.0) / (NUM_POINTS / 4.0);
//		}
//		/*
//		if(getRand() > .5)
//			xUpdates[i] = -getRand() * 10;
//		else
//			xUpdates[i] = getRand() * 15;
//		if(getRand() > .5)
//			yUpdates[i] = -getRand() * 10;
//		else
//			yUpdates[i] = getRand() * 15;
//		costUpdates[i] = 50;//getRand() * 50;*/
//	}
//
//	output.xUpdates = xUpdates;
//	output.yUpdates = yUpdates;
//	output.costUpdates = costUpdates;
//
//	Messages::CostMap.publish(&output);
//
//	/*output.numPoints = 5;
//	double xUpdates[] = {-50, 0, 50, 15, 100};
//	double yUpdates[] = {-50, 0, 50, 15, 100};
//	double costUpdates[] = {0, 10, 20, 30, 40};
//	output.xUpdates = xUpdates;
//	output.yUpdates = yUpdates;
//	output.costUpdates = costUpdates;
//	Messages::CostMap.publish(&output);*/
//	
//	StateEstimationType se;
//	se.Easting = -10;//0;
//	se.Northing = 0;
//	se.Heading = 3.141592653589793238462643383279500 / 2;
//	Messages::StateEstimation.publish(&se);
//
//	SetWaypointsType sw;
//	sw.numPoints = 1;
//	point2d waypoints[] = {point2d::spawn(50, 50)};
//	sw.points = waypoints;
//	Messages::SetWaypoints.publish(&sw);
//
//	IPC_disconnect();
//}