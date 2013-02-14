#include "LoadingMode.h"

LoadingMode::LoadingMode(ShooterTilt* s)
{
	shooterTilt = s;

	name = "LOAD";
	indicator = "** Loading Mode **";
}

LoadingMode::~LoadingMode()
{

}

void LoadingMode::begin(DriverStationLCD *screen)
{
	TeleopMode::begin(screen);
	screen->PrintfLine(
		DriverStationLCD::kUser_Line2,
		"Press A to Exit"
	);

	shooterTilt->goToPosition(loadingPosition);
}
