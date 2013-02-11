#ifndef SHOOTERTILT_H
#define SHOOTERTILT_H

#include "CANJaguar.h"
#include "DigitalInput.h"

class ShooterTilt {
public:
	
	ShooterTilt(CANJaguar m, DigitalInput s);
	~ShooterTilt();
	
	void goToPosition(int position);
	void goHome();
	void changePosition(int difference);
	
private:
	
	CANJaguar motor;
	DigitalInput counterSwitch;
	
};

#endif