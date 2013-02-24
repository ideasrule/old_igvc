#include "Frame.h"
#include "LaneDetector.h"
#include "ObstacleDetectorFactory.h"
#include "IObstacleDetector.h"
#include "VisionTiming.h"
#include "VisionIPC.h"
#include "ImageViewer.h"
#include "PerformanceTimer.h"
#include "FrameLoggerFactory.h"

#include "highgui.h"
#include <iostream>
#include <iomanip>
#include <conio.h> // for kbdhit

#include "commandArgs.h"
#include "Visualizations.h"

#define CAMERA_SHARED_PTR
#include "Camera.h"


using namespace std;
using namespace Pave_Libraries_Common;

int main(int argc, char **argv)
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	commandArgs *args = parseArgs(argc, argv);
	
	if (args->ipc) VisionIPC::connect(); 

	shared_ptr<Frame> frm;
	shared_ptr<Camera> cam(new CameraVidere("3114"));
	shared_ptr<FrameLogger> olog;
	shared_ptr<FrameLogger> ilog;
    if (args->logToDir) {
		olog = FrameLoggerFactory::createFrameLogWriter(args->logToDir);
    }
    if (args->readLogDir && args->readLogBasename) {
		ilog = FrameLoggerFactory::createFrameLogReader(
			args->readLogDir, args->readLogBasename);
    }

	if (!ilog) {
		cam->setPointCloudNeeded(true);
		cam->ensureSetupCamera();
	}
	shared_ptr<IObstacleDetector> obstacleDetector = ObstacleDetectorFactory::createManduchi();
	LaneDetector laneDetector(480,640);
	VisionTiming timing;
	VisionIPC ipc(ilog != NULL); // if reading logs, send state estimation updates

    CPerformanceTimer allFramesTimer;
    allFramesTimer.Start();

	cerr << "=====Hit any key to stop capturing.=====" << endl;
        
	while (!_kbhit() && (!args->ipc || VisionIPC::isConnected())) {
        timing.start(TIMER_OVERALL);

		if (args->ipc) {
			timing.start(TIMER_STATE_CAPTURE);
			ipc.checkForStateUpdate();
			timing.end(TIMER_STATE_CAPTURE);
		}

        timing.start(TIMER_CAPTURE);
		if (!ilog) {
			cam->changeTransformPitch(ipc.pitch);
			frm = cam->getNextImage();
			if (!frm)
				break;
		} else {
			frm = ilog->readLog(ipc.state);
			if (!frm)
				break;
			ipc.stateValid = true;
		}
        timing.end(TIMER_CAPTURE);

		timing.start(TIMER_OUTPUT);
        if (olog) {
			olog->log(frm, args->ipc ? ipc.state : NULL);
		}
		timing.end(TIMER_OUTPUT);

		if(!args->logOnly) {
			timing.start(TIMER_OBSTACLE_DETECTION);
			obstacleDetector->process(frm);
			timing.end(TIMER_OBSTACLE_DETECTION);

			if (args->detectLanes) {
				laneDetector.process(frm, timing);
			}

			if(args->ipc) {
				ipc.publish(frm, laneDetector.splines);
				//ipc.publish(frm, vector<GroundPolyLane<2>>());
			}

			if (args->showOverlayImage) {
				showDisparity(frm);
				cv::imshow("Internal Overlay", *(obstacleDetector->overlay()));
				// just for getting visualizations for the spie paper
				//cv::imwrite(std::string("C:/IGVC/Logs/2010-11-29-16-31-49-obstacle-out/obs-overlay-")
				//	+ frm->name + std::string(".png"), *(obstacleDetector->overlay()));
				//showTopDown(frm);
				if (args->detectLanes)
					showLaneOverlay(frm, laneDetector.splines, true);
			}

			if(args->topDown) {
				showTopDown(frm);
			}
		}
			
		timing.end(TIMER_OVERALL);

		if (args->time) {
			timing.printTo(cout);
		}

		if(args->topDown || args->showOverlayImage) cv::waitKey(0);
		else Sleep(0);
	}

    allFramesTimer.Stop();
    std::cerr << "All processing took " << allFramesTimer.Interval_S() << " s." << std::endl;

	if (!args->ipc) {
		cerr << "Hit any key to exit." << endl;
		while (!_kbhit() && cv::waitKey(50) == -1);	
	} else cv::waitKey(2000);

	return EXIT_SUCCESS;
}