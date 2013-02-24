#pragma once

#include "Memory.h"
#include "Spline.h"
#include "GroundPolyLane.h"
#include "Frame.h"
#include <vector>

class VisionIPCStateHandler;

namespace Pave_Libraries_Common {
	struct VisionUpdateType;
	struct StateEstimationType;
	struct CompassUpdateType;
}

using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Camera;

class VisionIPC
{
public:
	VisionIPC(bool publishVisionState = false);
	~VisionIPC();
	static void connect();
	static bool isConnected();
	static void disconnect();
	static void handleStateUpdate(void *data);
	static void handleCompassUpdate(void *data);
	bool checkForStateUpdate();
	void publish(const shared_ptr<Frame> frm, const std::vector<Spline> &splines);
	StateEstimationType *state;
	double pitch;
	bool stateValid;
	bool publishVisionState;
};
