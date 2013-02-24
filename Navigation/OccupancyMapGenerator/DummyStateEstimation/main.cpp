#include "ipc.h"
#include "Common.h"
#include <cmath>

const double myPI = 4*atan(1.0);  // the real one is missing!
const double angularVelocity = 5.0*myPI/16.0;
const double dt = 1/20.0;

void sendDummyStateEstimation(void *, unsigned long, unsigned long)
{
    static double theta;
    StateEstimationType state = { 3*sin(theta), 3*cos(theta), -theta };
    Messages::StateEstimation.publish(&state);
	theta += angularVelocity*dt;
    if(theta > 2*myPI) { theta -= 2*myPI; }
}


int main(int argc, char *argv[])
{
    IPCConnect("DummyStateEstimation");
    IPC_addPeriodicTimer((int) (1.0/dt), sendDummyStateEstimation, 0);
    IPC_dispatch();
}