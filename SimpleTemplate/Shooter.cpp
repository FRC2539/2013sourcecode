#include "Shooter.h"

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
}

void Shooter::fire()
{
	t_launch.Start();
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
	Wait(0.3);
	launcher->Set(DoubleSolenoid::kReverse);

	semGive(launcherSem);
}
