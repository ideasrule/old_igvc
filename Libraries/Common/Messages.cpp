/*
The class Messages in Messages.h contains the standardized set of IPC messages.
Each message named xMessage consists of:
	-An IPCMessage x which contains the message's name and format string
	IPCMessages will cleanly handle message definition, subscription, and publishing
	-A struct xType which contains the data the message will pass
*/

#include "IPCMessage.h"
#include "Messages.h"

namespace Pave_Libraries_Common
{
	// State Estimation Update

	IPCMessage& Messages::StateEstimation = IPCMessage(
		"State Estimation",
		//"{double, double, double, double, double, double, double, double}",
		"{double, double, double, double}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::PauseDisable = IPCMessage(
		"PauseDisable",
		"{boolean, boolean}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::CostMap = IPCMessage(
		"CostMap",
		"{double, double, double, double, double, double, int, <double: 7>, <double: 7>, <double: 7>}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::BumblebeeUpdate = IPCMessage(
		"BumblebeeUpdate",
		"{int, <double: 1>, <double: 1>, <double: 1>, int, int, <int: 6>, <float: 6>, <float: 5>, <float: 5>, {double, double, double, double, double, double, double, double}}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);
	/*
	IPCMessage& Messages::SplineIPC=IPCMessage(
		"SplineIPCUpdate",
		"{int, <int:1>,<int:1>}",
		IPC_VARIABLE_LENGTH,
		true,
		true
};*/
/*
	IPCMessage& Messages::VisionUpdate = IPCMessage(
		"VisionUpdate",
//		"{int, <double: 1>, <double: 1>, <double: 1>, int, int, <int: 6>, <float: 6>, <float: 5>, <float: 5>, {double, double, double, double, double, double, double, double}}",
//		"{int, <double: 1>, <double: 1>, <double: 1>, int, int, <float: 5>, <float: 6>, <float: 6>, {double, double, double, double, double, double, double, double}}",	
		"{int, <double: 1>, <double: 1>, <double: 1>, int, <{int, <float: 1>, <float: 1>}: 5}, {double, double, double, double, double, double, double, double}}",	
		IPC_VARIABLE_LENGTH,
		true,
		true
		);
*/

	IPCMessage& Messages::VisionUpdate=IPCMessage(
		"VisionUpdate",
		"{int,<double:1>,<double:1>,<double:1>,int,<{int,<float:1>,<float:1>}:5>,{double, double, double, double, double, double, double, double}}",
		IPC_VARIABLE_LENGTH,
		true,
		true
	);
	IPCMessage& Messages::PathUpdate = IPCMessage(
		"Path",
		"list_point3d",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::MaxSpeedUpdate = IPCMessage(
		"MaxSpeed",
		"double",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::SetWaypoints = IPCMessage(
		"SetWaypoints",
		"list_point2d",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::SetWheelSpeed = IPCMessage(
		"SetWheelSpeed",
		"{double, double}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::GetWheelSpeed = IPCMessage(
		"GetWheelSpeed",
		"{double, double}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

   IPCMessage& Messages::SetLight = IPCMessage(
		"SetLight",
		"{boolean}",
		4,
		false,
		true
		);

  // IPCMessage& Messages::SetEstop = IPCMessage(
		//"SetEstop",
		//"{boolean}",
		//4,
		//false,
		//true
		//);

   IPCMessage& Messages::GetEstop = IPCMessage(
		"GetEstop",
		"{boolean}",
		4,
		false,
		true
		);

   IPCMessage& Messages::ClearCostMap = IPCMessage(
		"ClearCostMap",
		"{boolean}",
		4,
		false,
		true
		);

  IPCMessage& Messages::HaveClearedCostMap = IPCMessage(
		"HaveClearedCostMap",
		"{boolean}",
		4,
		false,
		true
		);

  IPCMessage& Messages::JausEnabled = IPCMessage(
		"JausEnabled",
		"{boolean}",
		4,
		false,
		true
		);

	IPCMessage& Messages::GPSUpdate = IPCMessage(
		"GPSUpdate",
		"{double, double, double, double, boolean}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::RawGPSUpdate = IPCMessage(
		"RawGPSUpdate",
		"{double, double, double, double, boolean}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::CompassUpdate = IPCMessage(
		"CompassUpdate",
		"{double, double, double}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);

	IPCMessage& Messages::GyroUpdate = IPCMessage(
		"GyroUpdate",
		"{double}",
		IPC_VARIABLE_LENGTH,
		true,
		true
		);
}
