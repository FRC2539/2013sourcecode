#include "EasyController.h"

EasyController::EasyController(int inputID):
	controller(inputID)
{

}

EasyController::~EasyController()
{

}

bool EasyController::isClicked(int button)
{
	bool wasPressed = previousState(button);

	bool pressed = isPressed(button);

	if (pressed && pressed != wasPressed)
	{
		states[button] = true;
		return true;
	}

	return false;
}

bool EasyController::isPressed(int button)
{
	return controller.GetRawButton(button);
}

bool EasyController::isReleased(int button)
{
	bool wasPressed = previousState(button);

	bool pressed = isPressed(button);

	if ( ! pressed && pressed != wasPressed)
	{
		states[button] = false;
		return true;
	}

	return false;
}

float EasyController::getAxis(int axis)
{
	return controller.GetRawAxis(axis);
}

bool EasyController::previousState(int button)
{
	try {
		return states.at(button);		
	}
	catch (const out_of_range& oor) {
		return false;
	}
}
