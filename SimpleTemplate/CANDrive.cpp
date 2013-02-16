#include "CANDrive.h"

CANDrive::CANDrive(CANJaguar* l, CANJaguar* r)
{
	leftMotor = l;
	rightMotor = r;

	multiplier = 1;

	rightMotor->ConfigEncoderCodesPerRev(360);
	leftMotor->ConfigEncoderCodesPerRev(360);
}

CANDrive::~CANDrive()
{

}

void CANDrive::arcade(float x, float y)
{
	rightMotor->Set((x + y) * multiplier);
	leftMotor->Set((x - y) * multiplier);
}

void CANDrive::rotate(float degrees)
{
	CANJaguar::ControlMode oldMode = rightMotor->GetControlMode();

	setMode(CANJaguar::kPosition);
	arcade(0, degrees);

	setMode(oldMode);
}

void CANDrive::setMode(CANJaguar::ControlMode mode)
{
	if (mode == CANJaguar::kSpeed)
	{
		multiplier = maxRPMs;
	}
	else if (mode == CANJaguar::kPosition)
	{
		multiplier = rotationsPerDegree;
	}
	else
	{
		multiplier = 1;
	}

	rightMotor->ChangeControlMode(mode);
	leftMotor->ChangeControlMode(mode);
	rightMotor->EnableControl();
	leftMotor->EnableControl();
}

void CANDrive::setNeutral(CANJaguar::NeutralMode mode)
{
	rightMotor->ConfigNeutralMode(mode);
	leftMotor->ConfigNeutralMode(mode);
}
