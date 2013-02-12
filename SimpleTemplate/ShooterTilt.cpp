#include "ShooterTilt.h"
#include "WPILib.h"
#include <fstream>
#include <iostream>
#include <semLib.h>

using namespace std;

ShooterTilt::ShooterTilt(CANJaguar m, int t, int b, int c)
{
	motor = m;

	topSwitch(t);
	bottomSwitch(b);
	counterSwitch(c);

	ifstream settings(saveFile);
	if (settings.is_open())
	{
		settings >> currentPosition;
	}
	else
	{
		currentPosition = -1;
	}


}

ShooterTilt::~ShooterTilt()
{
	ofstream settings(saveFile);
	settings << currentPosition;
}

void ShooterTilt::goToPosition(int position)
{
	targetPosition = position;

	t_changeTilt.Start();
}

void ShooterTilt::goHome()
{
	targetPosition = -1;

	t_changeTilt.Start();
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

	// Get control of the tilt motor. If not available, ignore this change.
	semTake(tiltMotorSem, NO_WAIT);

	motor.Set(direction);
	bool wasPressed = isPressed(counterSwitch);
	bool pressed = false;

	while (currentPosition != targetPosition)
	{
		Wait(0.05);

		pressed = isPressed(counterSwitch);
		if (pressed && ! wasPressed)
		{
			currentPosition += direction;
			if (currentPosition < 0)
			{
				currentPosition = 100;
			}
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
	motor.Set(0);

	semGive(tiltMotorSem);
}

bool ShooterTilt::isPressed(DigitalInput& limitSwitch)
{
	return ! limitSwitch.Get();
}
