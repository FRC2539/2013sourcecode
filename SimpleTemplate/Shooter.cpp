#include "Shooter.h"
#include "WPILib.h"

Shooter::Shooter(CANJaguar f, CANJaguar b, DoubleSolenoid l)
{
	frontWheel = f;
	backWheel = b;
	launcher = l;

	t_launch('changeTilt', (FUNCPTR)s_launch);
}

Shooter::~Shooter()
{

}

void Shooter::setSpeed(double speed)
{
	frontWheel.Set(speed * -1);
	backWheel.Set(speed * -0.92);
}

double Shooter::getFrontSpeed()
{
	return frontWheel.Get() * -100;
}

double Shooter::getBackSpeed()
{
	return backWheel.Get() * -100;
}

void Shooter::fire()
{
	t_launch.Start();
}

int Shooter::s_launch(Shooter *shooter)
{
	shooter->launch();
	return 0;
}

void Shooter::launch()
{
	semTake(launcherSem, NO_WAIT);

	launcher.Set(DoubleSolenoid::kForward);
	Wait(0.3);
	launcher.Set(DoubleSolenoid::kReverse);

	semGive(launcherSem);
}
