#include "Spline.h"
#include "VisionIPC.h"
#include "ipc.h"
#include "tca.h"  // IPC
#include "Common.h"
#include <vector>

using namespace std;

static StateEstimationType *staticState;
static double staticCompassPitch;

static void doNothing() {}

void VisionIPC::connect() {
	IPCConnect("Vision");
	if (!isConnected()) exit(0);
	IPC_setVerbosity(IPC_Print_Warnings);
	x_ipcRegisterExitProc(&doNothing); // so we don't exit when Central is closed until the camera is closed
	staticState = new StateEstimationType();
	staticState->Northing = 0;
	staticState->Easting = 0;
	staticState->Heading = 0;
	staticState->Speed = 0;
	Messages::StateEstimation.subscribe(VisionIPC::handleStateUpdate, true, false);
	Messages::StateEstimation.setQueueLength(1);
	Messages::CompassUpdate.subscribe(VisionIPC::handleCompassUpdate, true, false);
	Messages::CompassUpdate.setQueueLength(1);
}
void VisionIPC::disconnect() {
	IPC_disconnect();
	delete staticState;
}
bool VisionIPC::isConnected() {
	return IPC_isConnected();
}
void VisionIPC::handleStateUpdate(void *data) {
	StateEstimationType *s = ((StateEstimationType*)data);
	*staticState = *s;
}

void VisionIPC::handleCompassUpdate(void *data) {
	CompassUpdateType *s = ((CompassUpdateType*)data);
	staticCompassPitch = Pave_Libraries_Conversions::Angle::RAD_DEG(s->pitch+0.04);
}

VisionIPC::VisionIPC(bool publishVisionState) {
	this->publishVisionState = publishVisionState;
	state = new StateEstimationType;
	stateValid = false;
	pitch = 0.0;
	/*
	update = new VisionUpdateType();
	update->numPoints = 0;
	update->xVals = NULL;
	update->yVals = NULL;
	update->costs = NULL;
	update->numLanePoints = 0;
	update->numLanes = 0;
	update->pointsPerLane = NULL;
	update->LanexVals = NULL;
	update->LaneyVals = NULL;
	update->state.Northing = 0;
	update->state.Easting = 0;
	update->state.Heading = 0;
	update->state.Speed = 0;
	hasState = false;
	*/
}
VisionIPC::~VisionIPC() {
	delete state;
}
bool VisionIPC::checkForStateUpdate() {
	//Block to get the latest and greatest state estimation.
	IPC_RETURN_TYPE ret = IPC_listenClear(100); //Need both listen calls
	ret = IPC_listenClear(100);
	*state = *staticState;
	pitch = staticCompassPitch;
	if (ret != IPC_Timeout) {
		stateValid = true;
		cerr << "VI checkForStateUpdate: Setting stateValid <- true" << endl;
		return true;
	} else {
		cerr << "VI checkForStateUpdate: Setting stateValid <- false" << endl;
		stateValid = false;
		return false;
	}
}
//algorithm: scan left at most 80 pixels; scan right at most 80 pixels; choose closest valid pixel, if any
bool estimate_position(const shared_ptr<Frame> frm, int x, int y, float &transformed_x, float &transformed_y) {
	int offset_right=0;
	int offset_left=0;
	bool found_left=false;
	bool found_right=false;
	int scan_x; //x value of closest valid pixel
	//scan right, then left
	for (; x+offset_right<640 && offset_right < 80 && !frm->validArr[y][x+offset_right]; offset_right++) {}
	if (x+offset_right != 640) found_right=true;
	for(; x-offset_left >=0 && offset_left<80 && !frm->validArr[y][x-offset_left]; offset_left++) {}
	if (x-offset_left >=0) found_left=true;

	if (found_left && found_right) {
		if (offset_left < offset_right) scan_x=x-offset_left;
		else scan_x=x+offset_right;
	}
	else if (found_left) scan_x=x-offset_left;
	else scan_x=x+offset_right;
	int ref_x=2*scan_x-x; /*calculate x value of pixel reflected about scan_x*/
	if (ref_x >=0 && ref_x < 640 && frm->validArr[y][ref_x] && !(bool)frm->obstacle[y][ref_x]) {
		transformed_y=frm->transformedCloud[y][scan_x].y;
		transformed_x=frm->transformedCloud[y][ref_x].x;
		return true;
	}
	return false;
	
}
void VisionIPC::publish(const shared_ptr<Frame> frm, const vector<Spline> &splines) {
	if (!isConnected()) {
		cerr << "IPC not connected!!!" << endl;
		return;
	}
	if (!stateValid) {
		cerr << "Invalid state, not publishing." << endl;
		return;
	}
	stateValid = false;
	cerr << "VI publish: Publishing, setting stateValid <- false" << endl;
	
	VisionUpdateType update;
	update.state = *state;
	
	// Compile obstacles
	vector<double> obsx, obsy, costs;
	for(int r = 0; r < frm->height; r++) {
		for(int c = 0; c < /*450*/frm->width; c++)	{	
			if(frm->validArr[r][c] && frm->obstacle[r][c]) {
				obsx.push_back(frm->transformedCloud[r][c].x);
				obsy.push_back(frm->transformedCloud[r][c].y);
				costs.push_back(1);
			}
		}
	}
	update.numPoints = obsx.size();
	update.xVals = update.numPoints ? &obsx[0] : NULL;
	update.yVals = update.numPoints ? &obsy[0] : NULL;
	update.costs = update.numPoints ? &costs[0] : NULL;

	/* old Compile lanes
	update.numLanes = lanes.size();
	update.numLanePoints = update.numLanes * 3;
	float *laneX = new float[update.numLanePoints];
	float *laneY = new float[update.numLanePoints];
	float *angle = new float[update.numLanes];

	for(int li = 0; li < lanes.size(); li++) {
		angle[li] = (float) lanes[li].theta;

		for(int i = 0; i < 3; i++) {
			laneX[li * 3 + i] = (float) lanes[li].pointsX[i];
			laneY[li * 3 + i] = (float) lanes[li].pointsY[i];
		}
	}
	update.laneAngles = update.numLanes ? angle : NULL;
	update.LanexVals = update.numLanePoints ? laneX : NULL;
	update.LaneyVals = update.numLanePoints ? laneY : NULL;
*/
	/*compile lanes for spline*/
	update.nsplines=splines.size();
	vector< vector<float> > all_x_params(update.nsplines);
	vector< vector<float> > all_y_params(update.nsplines);
	vector<struct SplineIPC> all_splines(update.nsplines);
	//cout<<"I got here"<<endl;
	for (int i=0; i < update.nsplines; i++) {
		int ncontrols=splines[i].controlPoints.size();
		for (int a=0; a < ncontrols; a++) {
			int x=splines[i].controlPoints[a].x;
			int y=splines[i].controlPoints[a].y;
			float transformed_x;
			float transformed_y;
			if (!frm->validArr[y][x]) {
				//cout<<"estimate activated"<<endl;
				bool estimate_successful=estimate_position(frm,x,y,transformed_x,transformed_y);
				if (!estimate_successful) break;
			}
			else {
				transformed_x=frm->transformedCloud[y][x].x;
				transformed_y=frm->transformedCloud[y][x].y;
			}
			all_x_params[i].push_back(transformed_x);
			all_y_params[i].push_back(transformed_y);
			//cout<<transformed_x<<" "<<transformed_y<<endl;
		}
		all_splines[i].numcontrols=all_x_params[i].size();
		//cout<<all_x_params[i].size()<<endl;
		if (all_splines[i].numcontrols != 0) {
			all_splines[i].xcontrols=&all_x_params[i][0];
			all_splines[i].ycontrols=&all_y_params[i][0];
		}
	}
	update.all_splines=&all_splines[0];
	Messages::VisionUpdate.publish(&update);
	cout << "Sending " << update.numPoints << " obstacle points and "
		<< update.nsplines << " lane splines." << endl;

	if(publishVisionState) {
		Messages::StateEstimation.publish(&update.state);
		cout << "Sending logged state estimation update." << endl;
	}

}
