#include "NJTransit.h"
#include "Common.h"
#include "ipc.h"

using namespace Pave_Libraries_Common;

double TranslationalSpeed;

int main(int argc, char **argv)
{
	bool retrieve = false;
	
	TranslationalSpeed = Data::getDouble("Translational_Speed", retrieve);

	IPC_connectModule("PathTracker","localhost");

	Messages::StateEstimation.subscribe(&StateEstimationHandler, true, false);

#ifndef DEBUGMAP
	Messages::CostMap.subscribe(&CostMapHandler, true, false);
#endif

	Messages::SetWaypoints.subscribe(&SetWaypointsHandler, true, false);
	Messages::StateEstimation.setQueueLength(1);
	Messages::SetWaypoints.setQueueLength(1);

	Messages::MaxSpeedUpdate.subscribe(&MaxSpeedHandler, true, false);
	Messages::MaxSpeedUpdate.setQueueLength(1);


#ifndef DEBUGMAP
	Messages::CostMap.setQueueLength(3);
#endif

	IPC_addPeriodicTimer(100, &FindPath, NULL);

	IPC_dispatch();

	return 0;
}
