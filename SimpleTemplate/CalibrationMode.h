#ifndef CALIBRATION_MODE_H
#define CALIBRATION_MODE_H

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

class CalibrationMode: public TeleopMode {
public:

	CalibrationMode(Shooter* shooter, ShooterTilt* tilt, Targeting* targeting, EasyController* controller);
	~CalibrationMode();

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
