#ifndef AUTOMATIC_MODE_H
#define AUTOMATIC_MODE_H

#include "TeleopMode.h"
#include "Targeting.h"
#include "EasyController.h"

class AutomaticMode: public TeleopMode {
public:

	AutomaticMode(Targeting* targeting, EasyController* controller);
	~AutomaticMode();

protected:

	Targeting *targeting;
	EasyController *controller;

};

#endif