#pragma once

#include <Windows.h>

BOOL ControlHandler(DWORD control);

void tick();

//int PWM(double percent);

double getWheelSpeed(int quadcount);//int counter, int direction, long dutyCycle, int *lastCount)

void setWheelSpeed(void *data);

void pauseDisable(void *data);

//void setEstop(void *data); //we cannot set the e-stop! only read it

void setLight(void *data);

double feedForwardMotor(double desSpeed, bool left);