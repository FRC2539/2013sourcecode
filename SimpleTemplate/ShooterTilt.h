#include "CANJaguar.h"
#include "DigitalInput.h"

class ShooterTilt{
public:
	
	ShooterTilt(CANJaguar m, DigitalInput s);
	~ShooterTilt();
	
	void goToPosition(int position);
	void goHome();
	void changeAngle(int difference);
	
private:
	
	CANJaguar motor;
	DigitalInput counterSwitch;
	
};