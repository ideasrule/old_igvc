#include "DriveByGamepad.h"
#include "ipc.h"
#include "Common.h"
#include "XInput.h"
#define _USE_MATH_DEFINES
#include "math.h"

using namespace Pave_Libraries_Common;

// time between input ticks
#define INPUT_TICK 50
#define WHEELBASE 0.6604
#define MAX_SPEED 1.9
#define MAX_OMEGA MAX_SPEED/WHEELBASE
#define MAX_SENSITIVITY 30
#define GAMEPAD_MAX +32767

bool light = false;
bool lightChange = false;
bool pause = true;
bool pauseChange = false;
int sensitivity = MAX_SENSITIVITY/10; // 0 - MAX_SENSITIVITY
double leftWheelSpeed;
double rightWheelSpeed;
bool pressingA = false;
bool pressingY = false;

void tick()
{
   if(!updateController())
   {
		printf("\nCONTROLLER NOT CONNECTED");
		leftWheelSpeed = 0;
		rightWheelSpeed = 0;
   }
   
   // send messages
   setWheelSpeed();
   if(lightChange)
   {
		lightChange = false;
		setLight();
   }
   if(pauseChange)
   {
		pauseChange = false;
		setPause();
   }
}

void setWheelSpeed()
{
	WheelSpeedType wheelSpeed;
	wheelSpeed.left = leftWheelSpeed;
    wheelSpeed.right = rightWheelSpeed;
	//printf("\rLeftWheelSpeed: %+3f, RightWheelSpeed: %+3f, Pause = %d, Light = %d",leftWheelSpeed,rightWheelSpeed,pause,light);

    Messages::SetWheelSpeed.publish(&wheelSpeed);
}

//void setEstop()
//{
//   SingleBoolType boolean;
//   boolean.value = estop;
//   Messages::SetEstop.publish(&boolean);
//}

void setLight()
{
   SingleBoolType boolean;
   boolean.value = light;
   Messages::SetLight.publish(&boolean);
}

void setPause()
{
   PauseDisableType pdT;
   pdT.Disable = true;
   pdT.Pause = pause;
   //Messages::PauseDisable.publish(&pdT);
}

int main(int argc, char **argv)
{
   // set up IPC
	IPC_connectModule("DriveByGamepad", "localhost");
	Timer::addTimer(tick, INPUT_TICK);
	IPC_dispatch();

   // teardown
	IPC_disconnect();
}

bool updateController()
{ 
   XINPUT_STATE state;
   
   if(XInputGetState(0, &state) != ERROR_SUCCESS)
      return false;

   XINPUT_GAMEPAD gamepad = state.Gamepad;
   if(gamepad.sThumbRX < +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
      gamepad.sThumbRX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
      gamepad.sThumbRX = 0;
   if(gamepad.sThumbRY < +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
      gamepad.sThumbRY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
      gamepad.sThumbRY = 0;
   bool a = gamepad.wButtons & XINPUT_GAMEPAD_A;
   bool y = gamepad.wButtons & XINPUT_GAMEPAD_Y;
   bool leftShoulder = gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER;
   bool rightShoulder = gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER;

   if(!pressingA && a)
   {
      pressingA = true;
      pause = !pause;
	  pauseChange = true;
   }
   else if(!a)
      pressingA = false;

   if(!pressingY && y)
   {
      pressingY = true;
      light = !light;
	  lightChange =	true;
   }
   else if(!y)
      pressingY = false;

   if(leftShoulder && sensitivity > 0)
      sensitivity--;
   if(rightShoulder && sensitivity < MAX_SENSITIVITY)
      sensitivity++;

  /* double speed = ((double)(gamepad.sThumbLX*gamepad.sThumbLX+gamepad.sThumbLY*gamepad.sThumbLY))/(GAMEPAD_MAX*GAMEPAD_MAX) *
                            sensitivity / MAX_SENSITIVITY * MAX_SPEED;
   leftWheelSpeed = speed * calculateLeftSpeedFraction(gamepad.sThumbLX, gamepad.sThumbLY);
   rightWheelSpeed = speed * calculateRightSpeedFraction(gamepad.sThumbLX, gamepad.sThumbLY);

   leftWheelSpeed = speed/MAX_SPEED;
   rightWheelSpeed = speed/MAX_SPEED;*/

	if(gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
	{
		rightWheelSpeed = 1.5;
		leftWheelSpeed = 1.5;
	}
	else if(gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
	{
		rightWheelSpeed += 0.01;
		leftWheelSpeed += 0.01;
		rightWheelSpeed = min(rightWheelSpeed,1.5);
		leftWheelSpeed = min(rightWheelSpeed,1.5);
	}
	else
	{
		double V = (double)gamepad.sThumbRY / GAMEPAD_MAX * MAX_SPEED;
		double Omega = (double)gamepad.sThumbRX / GAMEPAD_MAX * MAX_OMEGA;
		leftWheelSpeed = V + (Omega*WHEELBASE)/2;
		rightWheelSpeed = V - (Omega*WHEELBASE)/2;

		leftWheelSpeed *= (double)sensitivity / MAX_SENSITIVITY;
		rightWheelSpeed *= (double)sensitivity / MAX_SENSITIVITY;
	}

	return true;
}

double calculateLeftSpeedFraction(double x, double y)
{
   if(x >= 0 && y >= 0) // Q1
      return +1;
   if(x <= 0 && y <= 0) // Q3
      return -1;
   if(x >= 0) // Q4
      return +1 - atan2(-y, x)/M_PI_4;
   if(x <= 0) // Q2
      return -1 + atan2(y, -x)/M_PI_4;
   return 0;

   
}

double calculateRightSpeedFraction(double x, double y)
{
   if(x >= 0 && y <= 0) // Q4
      return -1;
   if(x <= 0 && y >= 0) // Q2
      return +1;
   if(x >= 0) // Q1
      return -1 + atan2(y, x)/M_PI_4;
   if(x <= 0) // Q3
      return +1 - atan2(-y, -x)/M_PI_4;
   return 0;
}