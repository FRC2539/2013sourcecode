#ifndef CALIBRATE_SYSTEMS_MODE_H
#define CALIBRATE_SYSTEMS_MODE_H

#include "TeleopMode.h"
#include "Targeting.h"
#include "EasyController.h"
#include "Shooter.h"
#include "ShooterTilt.h"

namespace RobotSystem {
	enum mode {
		rotation,
		shooter,
		select
	};
}

class CalibrateSystemsMode: public TeleopMode {
public:

	CalibrateSystemsMode(Shooter* shooter, ShooterTilt* tilt, Targeting* targeting, EasyController* controller);
	~CalibrateSystemsMode();

	void begin(DriverStationLCD *screen);
	void execute(DriverStationLCD *screen);
	void end();

protected:

	Shooter *shooter;
	ShooterTilt *tilt;
	Targeting *targeting;
	EasyController *controller;

	void setSystem(RobotSystem::mode newSystem);
	RobotSystem::mode currentSystem;

};

#endif
