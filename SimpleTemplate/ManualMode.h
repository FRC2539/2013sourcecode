#ifndef MANUAL_MODE_H
#define MANUAL_MODE_H

#include "TeleopMode.h"
#include "Shooter.h"
#include "ShooterTilt.h"
#include "EasyController.h"

class ManualMode: public TeleopMode {
public:

	ManualMode(Shooter* shooter, ShooterTilt* shooterTilt, EasyController* joystick);
	~ManualMode();
	
	void execute(DriverStationLCD* screen);

protected:

	Shooter *shooter;
	ShooterTilt *shooterTilt;
	EasyController *joystick;

	float oldShooterSpeed;
	float shooterSpeed;

	float getThrottleSpeed();

	bool optimal;
};

#endif
