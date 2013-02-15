#include "ManualMode.h"
#include <cmath>

ManualMode::ManualMode(Shooter* s, ShooterTilt* t, EasyController* j)
{
	shooter = s;
	shooterTilt = t;
	joystick = j;

	name = "MANUAL";
	indicator = "** MANUAL TARGETING **";

	oldShooterSpeed = getThrottleSpeed();
}

ManualMode::~ManualMode()
{

}

void ManualMode::execute(DriverStationLCD *screen)
{
	if (joystick->isClicked(Sidewinder::up))
	{
		shooterTilt.changePosition(1);
	}
	else if (joystick->isClicked(Sidewinder::down))
	{
		shooterTilt.changePosition(-1);
	}
	screen->PrintfLine(
		DriverStationLCD::kUser_Line3,
		"Shooter Position: %i",
		shooterTilt.getPosition()
	);

	shooterSpeed = getThrottleSpeed();

	if (abs(shooterSpeed - oldShooterSpeed) >= 0.01)
	{
		shooter.setSpeed(shooterSpeed);
		oldShooterSpeed = shooterSpeed;
		
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"Shooter Speed: %4.2d%%",
			shooterSpeed * 100
		);
	}

	if (joystick->isClicked(Sidewinder::trigger))
	{
		shooter.fire();
	}

}

float ManualMode::getThrottleSpeed()
{
	return (joystick->getAxis(Sidewinder::throttle) + 1) / 2;
}
