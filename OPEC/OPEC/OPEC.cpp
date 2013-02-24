#include "OPEC.h"
#include "ipc.h"
#include "Common.h"
#include "PerformanceTimer.h"
#include "Conversions.h"
#include "Labjack.h"
#include "MedianFilter.h"
#include <Windows.h>
#include "VehicleDynamics.h"
#include "SpeedController.h"
#include <ctime>

using namespace Pave_Libraries_Common;
using namespace Pave_Libraries_Conversions;
using namespace Pave_VehicleParameters;
using namespace Pave_OPEC_Labjack;

// time between input ticks
#define INPUT_TICK 30

//logging
#include <time.h>
#include <fstream>
#include <iostream>
std::ofstream file;

Labjack *labjack = NULL;

bool isDisabled;
bool isPaused;
bool pauseChanged;
bool disableChanged;
bool sirenOn;
bool sirenChanged;

//temp for testing:
//bool PWMincreasing = true;
double PWMpercent = 0;
double PWMout = 0;

double Yaw_Rate = 0;
double minRead = 0;
double maxRead = 0;
int iCount = 0;

LARGE_INTEGER previousTick;
LARGE_INTEGER currentTick;
LARGE_INTEGER frequency;

time_t startTime;

double feedForwardLeft(double desired);
double feedForwardRight(double desired);
SpeedController scLeft(true);
SpeedController scRight(false);
MedianFilter speedFilter_left, speedFilter_right;

CPerformanceTimer ptimer;


void tick()
{
	LJOutputs outputs;

	// read inputs from labjack
	LJInputs *inputs = labjack->readInputs();

	// calibrate gyros: AIN_0 is the reference voltage (2.5V)
	//                : AIN_1 is the rate output voltage (between 0V and 5V)

	Yaw_Rate = -((inputs->AIN_1 - inputs->AIN_0)/0.004979); // yaw rate in degrees per sec 
	//negation due to mounting orientation: RHR on gyro is opposite direction of positive compass angle
	printf("\rGyro %4f\t", inputs->AIN_1);
	//printf("yaw rate range: %4f to %4f\n", min(Yaw_Rate, minRead), max(Yaw_Rate, maxRead));
	minRead = min(Yaw_Rate, minRead);
	maxRead = max(Yaw_Rate, maxRead);

	GyroUpdateType *gut = new GyroUpdateType();
	gut->yawRate = Angle::DEG_RAD(Yaw_Rate);
	Messages::GyroUpdate.publish(gut);


	// step wheel speed for testing
	if(0) {
		static int behaviorIndex = 0;
		static double behaviorSpeeds[] = { 0, MaxWheelSpeed/2, 0, -MaxWheelSpeed/2, 0 };
		static double behaviorTimes[] = { 3, 10, 3, 10 };
		if(difftime(time(NULL), startTime) > behaviorTimes[behaviorIndex]) {
			behaviorIndex = (behaviorIndex + 1) % 4;
			time(&startTime);

			scLeft.setDesired(behaviorSpeeds[behaviorIndex]);
			scRight.setDesired(behaviorSpeeds[behaviorIndex]);
		}
	}

	
	bool EstopNow = !(inputs->MIO_0); //negation due to digital logic inversion
    bool PauseNow = !(inputs->MIO_1);
	
	//cout << " Pause signal: " << PauseNow << endl;

	if (PauseNow != isPaused)
	{
		isPaused = PauseNow;
		//isPaused = !isPaused;
		pauseChanged = true;
	}

	if(EstopNow != isDisabled)
	{
		isDisabled = EstopNow;
		disableChanged = true;
	}

	if(pauseChanged || disableChanged)
	{
		if(disableChanged)
		{
			if(isDisabled)
				cout << endl << "Disable" << endl;
			else
				cout << endl << "Enable" << endl;	
		}
		if(pauseChanged)
		{
			if(isPaused)
				cout << endl << "Pause" << endl;
			else
				cout << endl << "Run" << endl;	
		}
		pauseChanged = disableChanged = false;
		PauseDisableType pdMessage;
		pdMessage.Disable = isDisabled;
		pdMessage.Pause = isPaused;
		Messages::PauseDisable.publish(&pdMessage);
	}

	if(sirenChanged)
	{
		sirenChanged = false;
		outputs.send_MIO_2 = true;
		if(sirenOn)
			cout << "Light On" << endl;
		else
			cout << "Light Off" << endl;
	}
	else
	{
		outputs.send_MIO_2 = false;
	}
	outputs.MIO_2 = sirenOn?1:0;

	// update time
	previousTick = currentTick;
	QueryPerformanceCounter(&currentTick);

	// calculate wheel speed
	double leftWheelSpeed = getWheelSpeed(inputs->Timer_0);
	double rightWheelSpeed = -getWheelSpeed(inputs->Timer_2);
	//Negative sign in above is necessary to compensate for direction of encoder

	if(fabs(leftWheelSpeed) < UpperLimitWheelSpeed)
		speedFilter_right.addValue(rightWheelSpeed);
	if(fabs(rightWheelSpeed) < UpperLimitWheelSpeed)
		speedFilter_left.addValue(leftWheelSpeed);

	double rightWheelSpeed_Filt = speedFilter_right.getMedian();
	double leftWheelSpeed_Filt = speedFilter_left.getMedian();

	if(isDisabled || isPaused)
	{
		scLeft.reset();
		scRight.reset();
	}

	double leftOutput = scLeft.getOutput(leftWheelSpeed_Filt);
	double rightOutput = scRight.getOutput(rightWheelSpeed_Filt);

	//ptimer.Stop();
	//double time_interval = ptimer.Interval_mS();

	//Timer 4 is Left, non-inverted
	//Timer 5 is Right, inverted
	if(isDisabled/* || time_interval > 1000*/)   //don't receive commands for a second
	{
		outputs.Timer_4 = 0;
		outputs.Timer_5 = 0;
	}
	else
	{
		outputs.Timer_4 = leftOutput; 
		outputs.Timer_5 = -rightOutput; //Negative sign due to motor mounting direction

	}

	cout << "leftOutput: " << leftOutput <<  "\trightOutput: " << rightOutput << endl;

	outputs.send_Timer_4 = true;
	outputs.send_Timer_5 = true;

	labjack->sendOutputs(&outputs);

	// publish messages
	WheelSpeedType wheelSpeed;
	wheelSpeed.left = leftWheelSpeed_Filt;
	wheelSpeed.right = rightWheelSpeed_Filt;
	Messages::GetWheelSpeed.publish(&wheelSpeed);

	//logging
	file << isDisabled << "," << isPaused << "," << sirenOn << "," << scLeft.getDesired() << "," << leftWheelSpeed_Filt << "," << leftOutput << "," << scRight.getDesired() << "," << rightWheelSpeed_Filt << "," << rightOutput << "," << Yaw_Rate << "," << inputs->Timer_0 << "," << inputs->Timer_2 << endl;
	if(!isPaused)
		iCount++;
}

void setWheelSpeed(void *data)
{
	WheelSpeedType *wheelSpeed = (WheelSpeedType *)data;	
	double leftSpeed = min(max(wheelSpeed->left,-1*MaxWheelSpeed),MaxWheelSpeed);
	double rightSpeed = min(max(wheelSpeed->right,-1*MaxWheelSpeed),MaxWheelSpeed);
	if(isPaused || isDisabled)
		leftSpeed = rightSpeed = 0;
	
	scLeft.setDesired(leftSpeed);
	scRight.setDesired(rightSpeed);

	//ptimer.Start();
}


void setLight(void *data)
{
   SingleBoolType *boolean = (SingleBoolType *)data;

   if ((boolean->value==1) != sirenOn)
   {
	   sirenChanged = true;
	   sirenOn = (boolean->value == 1);
   }
}


void pauseDisable(void *data)
{
	PauseDisableType *pdT = (PauseDisableType *)data;
	//read in PauseDisable
	bool disable = pdT->Disable;
	bool pause = pdT->Pause;
	ptimer.Start();
}


int main(int argc, char **argv)
{
	IPC_setVerbosity(IPC_VERBOSITY_TYPE::IPC_Print_Warnings);
	QueryPerformanceFrequency(&frequency);
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ControlHandler, TRUE);

	time(&startTime);

	//logging
	time_t theTime;
	time(&theTime);
	tm *localTime = localtime(&theTime);
	char filename[1000];
	sprintf(filename, LOGGING_ROOT);
	sprintf(filename + strlen(LOGGING_ROOT), "%04d-%02d-%02d-%02d-%02d-%02d-OPEC.csv", localTime->tm_year + 1900, localTime->tm_mon + 1, localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);	
	file.open(filename);
	file << "Disabled,Paused,Siren,LeftDesired,LeftActual,LeftOutput,RightDesired,RightActual,RightOutput,YawRate,LeftTicks,RightTicks" << endl;

	ptimer.Start();

   // initialize labjack
   if(argc < 2)
      labjack = new Labjack(); // USB
   else
      labjack = new Labjack(argv[1]); // ETHERNET

   //set up Median Filter for speed estimates
	speedFilter_right = MedianFilter(3);
	speedFilter_left = MedianFilter(3);

   //Temporary Estop hack
	isDisabled = false; //to start enabled
	isPaused = true; //to start in pause
	sirenOn = false;

   // set up IPC
	IPC_connectModule("OPEC", "localhost");
	Messages::SetWheelSpeed.subscribe(setWheelSpeed, true, false);
	Messages::PauseDisable.subscribe(pauseDisable, true, false);
	//Messages::SetEstop.subscribe(setEstop, true, false); // no set e-stop message!
	Messages::SetLight.subscribe(setLight, true, false);
	Timer::addTimer(tick, INPUT_TICK);
	IPC_dispatch();

   // teardown
	IPC_disconnect();
	file.close();
	labjack->resetLabjack();
    delete labjack;
}

//Inputs are the counts since last update; quadrature direction as a bool; and duty cycle measurement
double getWheelSpeed(int quadcount)//int counter, int direction, long dutyCycle, int *lastCount)
{
	double elapsed = (double)(((long double)currentTick.QuadPart - (long double)previousTick.QuadPart)/(long double)frequency.QuadPart);

	return Speed::TPerSec_MPerSec(quadcount/elapsed);//ticks / elapsed);
}

BOOL ControlHandler(DWORD control)
{
	if(labjack != NULL)
		labjack->resetLabjack();
	exit(EXIT_SUCCESS);
	/*
	if(control == CTRL_C_EVENT || control == CTRL_CLOSE_EVENT)
		return TRUE;
	return FALSE;*/
}
