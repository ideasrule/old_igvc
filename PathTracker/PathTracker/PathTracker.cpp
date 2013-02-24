
#include "Common.h"
#include <iostream>
#include <cmath>

#include "VehicleDynamics.h"
#include "Path.h"

using namespace Pave_VehicleParameters;

//Pursuit point constants
#define LOOKAHEAD 1.0

//xtrack constants
#define FORWARD_DIST 0.45
#define BACKWARD_DIST 0.15

int usePursuitPoint = 0;
Path *path = NULL;
bool paused = true;
bool disabled = false;


bool largeYawFlag = false;
double TranslationalSpeed;
double maxSpeed = 0;
const double minTranslationSpeed = .4; //in m/s

//controller constants
const double kOm = 1.0; //1.8; //omega = k0 * phi
const double kE = 0.7; //1.3;
const double kSoft = 0.5;   //reduces oversensitivity to ctError at low speeds
const double kSoft2 = 0.3;  //make sure omega is not zero when desired speed is zero
const double kMax = .15;
const double maxYawRate = MaxYawRate * kMax;   

StateEstimationType *se = 0;
double lastSEHeading = 10;   //initialize to something invalid

double initialHeading;
bool clearingCostMap = false;
bool haveClearedCostMap = false;

double signedAngleToPoint(const wykobi::point2d<>& pursuitPoint, const wykobi::point2d<>& position, double heading)
{
	wykobi::point2d<> orientation(wykobi::sin<wykobi::Float>(heading),wykobi::cos<wykobi::Float>(heading));
	double ans = wykobi::oriented_vertex_angle(position+orientation, position, pursuitPoint )*wykobi::PIDiv180;
	if(ans > wykobi::PI)
		return ans - 2*wykobi::PI;
	return ans;
}
void StateUpdateHandler(void *data)
{

	se = (StateEstimationType *)data;
	lastSEHeading = se->Heading;
	wykobi::point2d<> currentPosition(se->Easting,se->Northing);

	double omega, delta, V;
	double phi = 0;

	//if (paused || disabled) {
	//	WheelSpeedType desiredSpeeds;
	//	desiredSpeeds.left = 0;
	//	desiredSpeeds.right = 0;
	//	Messages::SetWheelSpeed.publish(&desiredSpeeds);
	//	return;
	//}

	if (clearingCostMap) {
		////V = minTranslationSpeed;
		//if (path != NULL) {
		//	delete path;
		//	path = NULL;
		//}

		//V = -0.1;
		//omega = -0.8;
		//double angleFromInitial = se->Heading - initialHeading;
		//cout << "angle from initial = " << angleFromInitial << ", heading = " << se->Heading << ", initialHeading = " 
		//	<< initialHeading << endl;
		//static bool isSecondTime = false;
		//if (angleFromInitial < 0)
		//	angleFromInitial += 2 * wykobi::PI;
		//if (angleFromInitial > 3*wykobi::PI/4 && angleFromInitial < 5*wykobi::PI/4) {
		//	isSecondTime = true;
		//	cout << "isSecondTime = true!" << endl;
		//}
		//if (angleFromInitial < wykobi::PI/2 && isSecondTime) {
		//	clearingCostMap = false;
		//	//send ipc message
		//	haveClearedCostMap = true;
		//	Messages::HaveClearedCostMap.publish(&haveClearedCostMap);
		//	cout << "Cost map clearing turn completed!" << endl;
		//	isSecondTime = false;
		//	V = 0;
		//	omega = 0;
		//}

		//if(paused || disabled)
		//{
		//	V = 0;
		//	omega = 0;
		//	printf("paused or disabled.\n");
		//}
		//
		//WheelSpeedType desiredSpeeds;
		//desiredSpeeds.left = V - omega*TrackWidth/2.0;
		//desiredSpeeds.right = V + omega*TrackWidth/2.0;
		//Messages::SetWheelSpeed.publish(&desiredSpeeds);	
		//
		//return;
	} else {
		if (path == NULL) {
			WheelSpeedType desiredSpeeds;
			desiredSpeeds.left = 0;
			desiredSpeeds.right = 0;
			Messages::SetWheelSpeed.publish(&desiredSpeeds);	
		}
	}

	if(path == NULL)
		return;

	//needed for speed selection
	double ctError;
	pathPoint closest = path->closestPoint(currentPosition, &ctError);

	////Pursuit point method
	if(usePursuitPoint)
	{
		wykobi::point2d<> pursuitPoint = path->pointDownPath(currentPosition, LOOKAHEAD);

		phi = signedAngleToPoint(pursuitPoint, currentPosition, se->Heading);
		omega = kOm * phi; 
	}
	else
	{
	//Cross-track method (with averaged tangent estimate)
	
		wykobi::point2d<> forward = path->pointDownPath(closest, FORWARD_DIST);
		wykobi::point2d<> backward = path->pointDownPath(closest, -BACKWARD_DIST);

		//below changed 5/22/09 
		//phi is relative angle between robot heading and angle from lookBehind point to lookAhead point
		phi = signedAngleToPoint(forward , backward, se->Heading);

		if(wykobi::orientation(forward, backward, currentPosition)==wykobi::Clockwise)
			ctError = -ctError;

		//2008 Code
		//omega = phi + wykobi::atan(ctError);
		//omega *= 0.5;

		// 5/22/09 code

		//set yaw rate proportional to difference in angle
		double xe = wykobi::atan(kE * ctError / (kSoft + se->Speed));
		delta = phi + xe;
		omega = kOm * (kSoft2 + closest.v) * delta;
		cout << "phi = " << phi << ", delta = " << delta << ", xe = " << xe << ", omega = " << omega << ", yawflag = " << largeYawFlag;
		if (abs(delta) > 1.0) {  //in radians, untuned
			largeYawFlag = true;
		} else if (delta < 0.2 && largeYawFlag) {
			largeYawFlag = false;
		}


		
		//Limit yaw rates to feasible range
		if(omega > 0)
			omega = min(omega, MaxYawRate);
		else
			omega = max(omega, -MaxYawRate); 
	}
//
///* Find velocity */
//	if(paused || disabled)
//	{
//		V = 0;
//		omega = 0;
//		printf("paused or disabled.\n");
//	}
//	else
	{
		//limit yawrate
		if(omega > 0)
			omega = min(omega, maxYawRate);
		else
			omega = max(omega, -maxYawRate);
		
		//new speed selection

		if (!largeYawFlag) {
			// first find min velocity based on omega
			double V_omega = 0.6/(0.2+phi*phi);
			//newer speed selection
			double cutoff = 0.6; //radians
			double alpha = 4.0; // sharpness of edge, must be even
			//double V_omega = TranslationalSpeed * exp(pow(phi / cutoff, alpha)) + minTranslationSpeed;
			V = max(minTranslationSpeed, min(closest.v, max(minTranslationSpeed, V_omega)));
			//V = min(closest.v, max(minTranslationSpeed, TranslationalSpeed * (1-(fabs(omega)/maxYawRate))));

		} else {
			V = minTranslationSpeed;
		}


		//double speedThresholdAngle = 0.8;    //angle in radians
		//if (fabs(delta) > speedThresholdAngle) {   
		//	V = minTranslationSpeed;
		//}

		//old speed selection
		//V = max(minTranslationSpeed, TranslationalSpeed * (1-(abs(omega)/maxYawRate)));

		
		//Limit V if it is unachievable given desired yaw rate
		double Vmax = VehicleDynamics::MaxFwdSpeedTurn(omega);
		if(abs(V) > Vmax)
			V = (V>0?1.0:-1.0)*Vmax;
	}

	cout << " V = " << V << endl;

	WheelSpeedType desiredSpeeds;
	desiredSpeeds.left = V - omega*TrackWidth/2.0;
	desiredSpeeds.right = V + omega*TrackWidth/2.0;
	Messages::SetWheelSpeed.publish(&desiredSpeeds);
}

void ClearCostMapHandler(void *data)
{
	if (*((bool*)data) == false)
		return;
	if (lastSEHeading > 9.99)
		return;

	//to a 360 degree turn
	clearingCostMap = true;
	initialHeading = lastSEHeading;
	cout << "Clearing Message Received, initialHeading = " << initialHeading << endl;

}


void MaxSpeedHandler(void *data)
{
	maxSpeed = *((double*)data);
	printf("MaxSpeed: %f\n", maxSpeed);
	maxSpeed = 0;
}

void PathUpdateHandler(void *data)
{
	PathUpdateType *newpath = (PathUpdateType *)data;

	if(path != NULL)
		delete path;
	path = new Path(*newpath);

}

void RunPauseHandler(void *data)
{
	PauseDisableType *pdUpdate = (PauseDisableType*)data;
	paused = pdUpdate->Pause == TRUE;
	disabled = pdUpdate->Disable == TRUE;
}

int main(int argc, char* argv[])
{

//	PathUpdateType pat;
//	pat.numPoints = 4;
//	pat.points = new point2d[4];
	//pat.points[0] = point2d::spawn(-5.87,-16.13);
	//pat.points[1] = point2d::spawn(-4.228,-19.36);
	//pat.points[2] = point2d::spawn(-1.398,-20);
	//pat.points[3] = point2d::spawn(6.571,-13.07);

//	pat.points[3] = point2d::spawn(-5.87,-16.13);
//	pat.points[2] = point2d::spawn(-4.228,-19.36);
//	pat.points[1] = point2d::spawn(-1.398,-20);
//	pat.points[0] = point2d::spawn(6.571,-13.07);
	
//	path = new Path(pat);

	bool retrieve = false;
	TranslationalSpeed = Data::getDouble("Translational_Speed", retrieve);
	if(!retrieve)
		return 0;

	//Connect to IPC
	IPC_connectModule("PathTracker","localhost");

	try
	{
		//Subscribe to state estimation

		Messages::StateEstimation.subscribe(StateUpdateHandler,true,false);
		Messages::PathUpdate.subscribe(PathUpdateHandler,true,false);
		Messages::PauseDisable.subscribe(RunPauseHandler, true, false);
		Messages::MaxSpeedUpdate.subscribe(MaxSpeedHandler, true, false);
		Messages::ClearCostMap.subscribe(ClearCostMapHandler, true, false);

		IPC_dispatch();

	}
	catch( exception &e)
	{
		cout << e.what() << endl;
	}
	catch(...)
	{
		cout << "non-exception error" << endl;
	}

	// teardown
	IPC_disconnect();
	return 0;
}