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
	bool wasPressed = states[button];

	bool pressed = isPressed(button);

	if (pressed && pressed != wasPressed)
	{
		wasPressed = true;
		return true;
	}

	return false;
}

bool EasyController::isPressed(int button)
{
	return getButton(button);
}

bool EasyController::isReleased(int button)
{
	bool wasPressed = states[button];

	bool pressed = isPressed(button);

	if ( ! pressed && pressed != wasPressed)
	{
		wasPressed = false;
		return true;
	}

	return false;
}

float EasyController::getAxis(int axis)
{
	return controller.GetRawAxis(axis);
}
