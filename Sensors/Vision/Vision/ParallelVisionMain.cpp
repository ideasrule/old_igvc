
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "Camera.h"
#include "Frame.h"
#include "LaneDetector.h"
#include "ObstacleDetector.h"
#include "VisionTiming.h"
#include "VisionIPC.h"
#include "commandArgs.h"
#include "omp_guard.hpp"

#include <iostream>
#include "windows.h" // for Sleep in milliseconds
#include <conio.h>
#include <highgui.h>

using namespace std;
using namespace Pave_Libraries_Common;

#define VISION_PERIOD 800
#define VISION_THREADS 4

int main(int argc, char **argv)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	commandArgs *args = parseArgs(argc, argv);
	bool exception = false;

	omp_lock_t cameraLock, ipcLock, guiLock;
	omp_init_lock(&cameraLock);
	omp_init_lock(&ipcLock);
	omp_init_lock(&guiLock);
	
	if (args->ipc) VisionIPC::connect();

	Camera *cam = new CameraVidere("3114");
	cam->setPointCloudNeeded(true);
	cam->ensureSetupCamera();
	shared_ptr<Frame> frm;

	// Profile to get period so we can offset the threads
	VisionTiming profileTiming, profileSum;
	int profiles = 0;

#pragma omp parallel num_threads(VISION_THREADS)
	{
		try {
			int i = omp_get_thread_num();
			ostringstream labelStream;
			labelStream << "T" << i;
			string label = labelStream.str();

			ObstacleDetector obstacleDetector;
			LaneDetector laneDetector(480,640);
			VisionTiming timing;
			timing.name = label.c_str();
			VisionIPC ipc;
			
			Sleep(VISION_PERIOD/VISION_THREADS*i);

			while (!_kbhit() && (!args->ipc || VisionIPC::isConnected())) {
				timing.start(TIMER_OVERALL);

				if (args->ipc) {
					omp_guard ipcGuard(ipcLock);
					//CRITICAL REGION
					if (!VisionIPC::isConnected())
						break;

					timing.start(TIMER_STATE_CAPTURE);
					ipc.checkForStateUpdate();
					timing.end(TIMER_STATE_CAPTURE);
					//END CRITICAL REGION (ipcGuard out of scope)
				}

				{
					omp_guard cameraGuard(cameraLock);
					//CRITICAL REGION
					timing.start(TIMER_CAPTURE);
					frm = cam->getNextImage();
					if (!frm)
						break;
					timing.end(TIMER_CAPTURE);
					if (!args->time) cout << i << flush;
					//END CRITICAL REGION (cameraGuard out of scope)
				}

				timing.start(TIMER_OBSTACLE_DETECTION);
				obstacleDetector.process(frm);
				timing.end(TIMER_OBSTACLE_DETECTION);

				if (args->detectLanes) {
					laneDetector.process(frm, timing);
				}

				if (args->ipc) {
					omp_guard ipcGuard(ipcLock);
					//CRITICAL REGION
					ipc.publish(frm, laneDetector.lanesVerified);
					//END CRITICAL REGION (ipcGuard out of scope)
				}

				timing.end(TIMER_OVERALL);

				timing.start(TIMER_OUTPUT);

#pragma omp critical (TIMING)
				if (args->time) {
					timing.printTo(cout, i == 0);
				}

#pragma omp master
				if (args->showingSomething) {

				}

				timing.end(TIMER_OUTPUT);

				// Make every thread take an integral number of VISION_PERIOD's between snapshots
				{
					int timeSpent = timing.timers[TIMER_OUTPUT] + timing.timers[TIMER_OVERALL];
					int timeToWait = 0;
					while (timeSpent > VISION_PERIOD) {
						timeSpent -= VISION_PERIOD;
						timeToWait += VISION_PERIOD;
					}
					timeToWait += VISION_PERIOD - timeSpent;
					if (timeToWait > VISION_PERIOD) {
						cerr << "VISION_PERIOD shorter than required time; about to drop frame" << endl;
					}
					Sleep(timeToWait);
				}
			}
		} catch (...) {
#pragma omp critical (EXCEPTION)
			exception = true;
		}
	} // end parallel

	delete cam;
	VisionIPC::disconnect();

	if (exception) cerr << "Exception thrown. ";
	
	//if (!args->ipc) {
		cerr << "Hit any key to exit." << endl;
		while (!_kbhit() && cv::waitKey(50) == -1);
	//}

	return exception ? EXIT_FAILURE : EXIT_SUCCESS;
}