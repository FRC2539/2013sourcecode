#include "ShooterTilt.h"
#include "SettingsCache.h"
#include "Commands/WaitCommand.h"
#include <fstream>
#include <iostream>

using namespace std;

ShooterTilt::ShooterTilt(CANJaguar* m, int t, int b, int c):
	t_changeTilt("changeTilt", (FUNCPTR)s_changeTilt),
	topSwitch(t),
	bottomSwitch(b),
	counterSwitch(c)
{
	inMotion = false;
	
	saveFile = "tilt_position.txt";
	
	motor = m;	

	SettingsCache::getByReference("tilt_position", currentPosition, -1);
	lastDirection = 0;
}

ShooterTilt::~ShooterTilt()
{
	SettingsCache::setValue("tilt_position", currentPosition);
}

void ShooterTilt::goToPosition(int position)
{
	if (! inMotion)
	{
		targetPosition = position;

		t_changeTilt.Start();
	}
}

void ShooterTilt::goHome()
{
	goToPosition(-1);
}

void ShooterTilt::changePosition(int difference)
{
	goToPosition(currentPosition + difference);
}

int ShooterTilt::getPosition()
{
	return currentPosition;
}

int ShooterTilt::s_changeTilt(ShooterTilt* shooter)
{
	shooter->changeTilt();
	return 0;
}


void ShooterTilt::changeTilt()
{
	int direction = 0;
	if (targetPosition < currentPosition)
	{
		direction = -1;
	}
	else if (targetPosition > currentPosition)
	{
		direction = 1;
	}
	else
	{
		return;
	}
	
	// Handle case where position has drifted past counterSwitch. Hopefully not
	// a common scenario.
	if (lastDirection == -1 && direction == 1 && ! isPressed(counterSwitch))
	{
		currentPosition--;
	}
	else if (lastDirection == 1 && direction == -1 && ! isPressed(counterSwitch))
	{
		currentPosition++;
	}

	// Get control of the tilt motor. If not available, ignore this change.
	semTake(tiltMotorSem, NO_WAIT);
	inMotion = true;

	motor->Set(direction);
	bool wasPressed = isPressed(counterSwitch);
	bool pressed = false;

	while (targetPosition == -1 || currentPosition != targetPosition)
	{
		WaitCommand(0.05);

		pressed = isPressed(counterSwitch);
		if (pressed && ! wasPressed)
		{
			currentPosition += direction;
		}

		wasPressed = pressed;

		if (isPressed(bottomSwitch) && direction < 0)
		{
			targetPosition = 0;
			currentPosition = 0;
		}
		else if (isPressed(topSwitch) && direction > 0)
		{
			targetPosition = currentPosition;
		}
	}
	motor->Set(0);

	inMotion = false;
	semGive(tiltMotorSem);
}

bool ShooterTilt::isPressed(DigitalInput& limitSwitch)
{
	return ! limitSwitch.Get();
}
