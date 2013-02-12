#ifndef SHOOTERTILT_H
#define SHOOTERTILT_H

#include "CANJaguar.h"
#include "DigitalInput.h"
#include "Task.h"
#include "Synchronized.h"

class ShooterTilt {
public:

	ShooterTilt(CANJaguar motor, int topSwitch, int bottomSwitch, int counterSwitch);
	~ShooterTilt();

	void goToPosition(int position);
	void goHome();
	void changePosition(int difference);

	int getPosition();

protected:

	static int s_changeTilt(ShooterTilt *shooter);
	void changeTilt();
	bool isPressed(DigitalInput& limitSwitch);

	CANJaguar motor;
	DigitalInput counterSwitch;
	DigitalInput topSwitch;
	DigitalInput bottomSwitch;

	int currentPosition;
	int targetPosition;
	bool inMotion = false;

	int saveFile = 'tilt_position.txt';

	Task t_changeTilt;
	SEM_ID tiltMotorSem;
};

#endif
