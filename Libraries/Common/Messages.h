/*
The class Messages in Messages.h contains the standardized set of IPC messages.
Each message named xMessage consists of:
	-An IPCMessage x which contains the message's name and format string
	IPCMessages will cleanly handle message definition, subscription, and publishing
	-A struct xType which contains the data the message will pass
*/

// USE INTS INSTEAD OF BOOLS

#pragma once
#include "IPCMessage.h"
#include "Geometry.h"
using namespace Pave_Libraries_Common;

namespace Pave_Libraries_Common
{
	// Container class for all pre-defined messages
	class Messages
	{
	public:
		static IPCMessage& StateEstimation;
		static IPCMessage& PauseDisable;
		static IPCMessage& CostMap;
        static IPCMessage& ClearCostMap;
		static IPCMessage& HaveClearedCostMap;
		static IPCMessage& PathUpdate;
		static IPCMessage& MaxSpeedUpdate;
		static IPCMessage& SetWaypoints;
		static IPCMessage& BumblebeeUpdate; // old shit
		static IPCMessage& VidereUpdate; // old shit

		static IPCMessage& VisionUpdate;
		static IPCMessage& GPSUpdate;
		static IPCMessage& RawGPSUpdate;
		static IPCMessage& CompassUpdate;
		static IPCMessage& GyroUpdate;
		static IPCMessage& SetWheelSpeed;
		static IPCMessage& GetWheelSpeed;
		//static IPCMessage& SetEstop;
		static IPCMessage& GetEstop;
		static IPCMessage& SetLight;
		static IPCMessage& JausEnabled;
	};

	// State Estimation Update
	struct StateEstimationType
	{
		double Northing;
		double Easting;
		double Heading; // in radians from north, 0-2pi clockwise
		double Speed;
		//double Delta;
		//double RightWheel;
		//double LeftWheel;
		//double YawRate;
	};

	// Pause and Disable Signals
	struct PauseDisableType
	{
		int Pause;
		int Disable;
	};

	struct MaxSpeedUpdate
	{
		double speed;
	};

	// Cost Map Update
	struct CostMapType
	{
		// CostMap header (so another service can construct its own map)
		double xMin;
		double xMax;
		double xUnitSize;
		double yMin;
		double yMax;
		double yUnitSize;

		// Updated points
		int numPoints;
		double *xUpdates;
		double *yUpdates;
		double *costUpdates;
	};

	struct BumblebeeUpdateType
	{
		int numPoints;
		double *xVals;
		double *yVals;
		double *costs;
		int numLanePoints;
		int numLanes;
		int *pointsPerLane;
        float *LaneConfidences;
		float *LanexVals;
		float *LaneyVals;
		StateEstimationType state;
	};

	struct SplineIPC {
		int numcontrols;
		float* xcontrols;
		float* ycontrols;
	};
	struct VisionUpdateType
	{
		int numPoints;
		double *xVals;
		double *yVals;
		double *costs;

		// lane stuff
		/*
		int numLanes;
		int numLanePoints;
		float *laneAngles;
		float *LanexVals;
		float *LaneyVals;
*/
		/*new lane stuff begins*/
		int nsplines;
		//int spline_array_size; //size in bytes
		struct SplineIPC* all_splines; /*will be sent as a char array*/
		/*new lane stuff ends*/
		
		StateEstimationType state;
	};
	
	typedef Pave_Libraries_Geometry::list_point3d PathUpdateType;   //x, y, v

	typedef Pave_Libraries_Geometry::list_point2d SetWaypointsType;

	struct GPSUpdateType
	{
		double latitude;
		double longitude;
		double speed; // in m/s
		double heading; // in radians from north, 0-2pi clockwise
		int validFix;
	};

	typedef struct GPSUpdateType RawGPSUpdateType;

	struct CompassUpdateType
	{
		double heading; // in radians from north, 0-2pi clockwise
		double pitch; //in radians from level, positive is tipping the front upwards
		double roll; //in radians from level, positive is tipping rightward
	};

	struct GyroUpdateType
	{
		double yawRate;
	};

	struct WheelSpeedType
	{
		// wheel speed in m/s (can be pos or neg)
		double left; 
		double right;
	};

   struct SingleBoolType
   {
      int value;
   };

   typedef struct SingleBoolType SetEstopType;
   typedef struct SingleBoolType GetEstopType;
   typedef struct SingleBoolType SetLightType;
   typedef struct SingleBoolType ClearCostMapType;
   typedef struct SingleBoolType JausEnabledType;
}
