#pragma once

void tick();

void setWheelSpeed();

void setEstop();

void setLight();

void setPause();

int main(int argc, char **argv);

bool updateController();

double calculateLeftSpeedFraction(double x, double y);

double calculateRightSpeedFraction(double x, double y);