#ifndef SHOOTERTILT_H
#define SHOOTERTILT_H

#include "CANJaguar.h"
#include "DigitalInput.h"
#include "Task.h"
#include "Synchronized.h"

class ShooterTilt {
public:

	ShooterTilt(CANJaguar* motor, int topSwitch, int bottomSwitch, int counterSwitch);
	~ShooterTilt();

	void goToPosition(int position);
	void goHome();
	void changePosition(int difference);

	int getPosition();

protected:

	static int s_changeTilt(ShooterTilt *shooter);
	void changeTilt();
	bool isPressed(DigitalInput& limitSwitch);

	CANJaguar* motor;
	Task t_changeTilt;
	
	DigitalInput topSwitch;
	DigitalInput bottomSwitch;
	DigitalInput counterSwitch;
	
	int currentPosition;
	int targetPosition;
	bool inMotion;

	char *saveFile;

	
	SEM_ID tiltMotorSem;
};

#endif
