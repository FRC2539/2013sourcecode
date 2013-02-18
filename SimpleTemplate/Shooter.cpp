#include "Shooter.h"
#include "Commands/WaitCommand.h"

Shooter::Shooter(CANJaguar* f, CANJaguar* b, DoubleSolenoid* l):
	t_launch("launch", (FUNCPTR)s_launch)
{
	frontWheel = f;
	backWheel = b;
	launcher = l;
}

Shooter::~Shooter()
{

}

void Shooter::setSpeed(double speed)
{
	frontWheel->Set(speed * 1);
	backWheel->Set(speed * 0.92);

	currentSpeed = speed;
}

void Shooter::setOptimalSpeed()
{
	setSpeed(optimalSpeed);
}

void Shooter::fire()
{
	t_launch.Start();
}

float Shooter::getCurrentSpeed()
{
	return currentSpeed * 100;
}

int Shooter::s_launch(Shooter *shooter)
{
	shooter->launch();
	return 1;
}

void Shooter::launch()
{
	semTake(launcherSem, NO_WAIT);

	launcher->Set(DoubleSolenoid::kForward);
	WaitCommand(0.3);
	launcher->Set(DoubleSolenoid::kReverse);

	semGive(launcherSem);
}
