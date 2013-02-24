#include "SpeedController.h"
#include "VehicleDynamics.h"
#include "Common.h"

#include <string>

static double getDoubleValue(const char *name)
{
	bool retrieve = false;
	double d = Data::getDouble(name, retrieve);

	std::string string_message("no value with name ");
	string_message += std::string(name);

	if(!retrieve)
		throw exception(string_message.c_str());

	return d;
}

SpeedController::SpeedController(bool left)
{
    // retrieve infro from config file
	const char *pstr, *istr;

	bool retrieve = false;
	if(left) {
		pstr = "P_Left";
		istr = "I_Left";
		this->ff = feedForwardLeft;
	}
	else {
		pstr = "P_Right";
		istr = "I_Right";
		this->ff = feedForwardRight;
	}

	double p = getDoubleValue(pstr);
	double i = getDoubleValue(istr);

	this->pid = new PID(p, i, 0.0);
}

SpeedController::~SpeedController(void)
{
	if(this->pid) delete this->pid;
}

void SpeedController::reset(void)
{
	setDesired(0.0);
	pid->resetIterm();
}

void SpeedController::setDesired(double desired)
{
	pid->setDesired(desired);
}

double SpeedController::getDesired(void)
{
	return pid->desired;
}

double SpeedController::getOutput(double wheelSpeed)
{
	return ff(pid->desired) + pid->getOutput(wheelSpeed);
}

//these functions are open-loop feedforwards for the motors
//because the motors are mounted in opposite directions, bias of the motor windings is a problem
//therefore two functions are needed
double SpeedController::feedForwardLeft(double desSpeed)
{
	double tau = 0;
	double K = 0;
	//left forward
	if(desSpeed > 0)
	{
		K = 0.0325;
		tau = 0.8994;
	}
	//left reverse
	else if(desSpeed < 0)
	{
		K = -0.02503;
		//K = 0.01000;
		tau = -0.9072;
	}
	else
		return 0;

	return min(max(.8*K*exp(tau*desSpeed),-1),1);
}

double SpeedController::feedForwardRight(double desSpeed)
{
	double tau = 0;
	double K = 0;
	//right reverse
	if(desSpeed < 0)
	{
		K = -0.0325;
		tau = -0.8994;
	}
	//right forward
	else if(desSpeed > 0)
	{
		K = 0.02503;
		//K = 0.01000;
		tau = 0.9072;
	}
	else
		return 0;

	return min(max(.8*K*exp(tau*desSpeed),-1),1);
}
