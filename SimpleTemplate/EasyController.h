#ifndef EASY_CONTROLLER_H
#define EASY_CONTROLLER_H

#include "Joystick.h"
#include <unordered_map>

using namespace std;

class EasyController {
public:

	EasyController(int inputID);
	~EasyController();

	bool isClicked(int button);
	bool isPressed(int button);
	bool isReleased(int button);

	float getAxis(int axis);

protected:

	Joystick controller;

	unordered_map<int, bool> states;
};

#endif
