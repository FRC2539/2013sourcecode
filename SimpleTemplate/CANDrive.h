#ifndef CAN_DRIVE_H
#define CAN_DRIVE_H

#include "CANJaguar.h"

class CANDrive {
public:

	CANDrive(CANJaguar *leftMotor, CANJaguar *rightMotor);
	~CANDrive();

	void arcade(float x, float y);
	void rotate(float degrees);

	void setNeutral(CANJaguar::NeutralMode mode);
	void setMode(CANJaguar::ControlMode mode);

protected:

	CANJaguar *leftMotor;
	CANJaguar *rightMotor;

	static float rotationsPerDegree = 0.07;
	static float maxRPMs = 100;

	float multiplier;
};

#endif
