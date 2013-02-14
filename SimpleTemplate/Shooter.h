#ifndef SHOOTER_H
#define SHOOTER_H

#include "CANJaguar.h"
#include "DoubleSolenoid.h"
#include "Task.h"
#include "Synchronized.h"

class Shooter {
public:

	Shooter(CANJaguar* frontWheel, CANJaguar* backWheel, DoubleSolenoid* launcher);
	~Shooter();

	void setSpeed(double speed);
	void fire();

protected:

	static int s_launch(Shooter *shooter);
	void launch();

	CANJaguar *frontWheel;
	CANJaguar *backWheel;
	DoubleSolenoid *launcher;

	Task t_launch;
	SEM_ID launcherSem;
};

#endif
