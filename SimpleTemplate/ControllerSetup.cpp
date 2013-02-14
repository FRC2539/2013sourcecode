#ifndef CONTROLLER_SETUP
#define CONTROLLER_SETUP

namespace Controller {
	enum axis {
		leftStickX,
		leftStickY,
		rightStickX,
		rightStickY,
		leftTrigger,
		rightTrigger
	};

	enum button {
		leftBumper,
		rightBumper,
		start,
		select,
		mode,
		up,
		down,
		left,
		right,
		X,
		Y,
		A,
		B
	};
};

namespace Joystick {
	enum axis {
		X = 1,
		Y = 2,
		throttle = 3
	};

	enum button {
		up = 3,
		down = 2,
		left,
		right,
		leftFront,
		leftBack,
		rightFront,
		rightBack,
		trigger = 1
	};
};

#endif
