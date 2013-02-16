#ifndef LOADING_MODE_H
#define LOADING_MODE_H

#include "TeleopMode.h"
#include "ShooterTilt.h"

class LoadingMode: public TeleopMode {
public:

	LoadingMode(ShooterTilt *shooterTilt);
	~LoadingMode();
	
	void begin(DriverStationLCD* screen);

protected:

	ShooterTilt *shooterTilt;
	const static int loadingPosition = 10;

};

#endif
