#ifndef TELEOP_MODE_H
#define TELEOP_MODE_H

#include "DriverStationLCD.h"
#include "ControllerSetup.cpp"

class TeleopMode {
public:

	TeleopMode();
	virtual ~TeleopMode();

	virtual void begin(DriverStationLCD* screen);
	virtual void execute(DriverStationLCD* screen);
	virtual void end();
	
	bool operator==(TeleopMode& other);

protected:

	char *name;
	char *indicator;
};

#endif
