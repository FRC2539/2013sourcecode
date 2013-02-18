#include "TeleopMode.h"

TeleopMode::TeleopMode()
{

}

TeleopMode::~TeleopMode()
{

}

void TeleopMode::begin(DriverStationLCD* screen)
{
	screen->PrintfLine(
		DriverStationLCD::kUser_Line1,
		indicator
	);
}

void TeleopMode::execute(DriverStationLCD* screen)
{

}

void TeleopMode::end()
{

}

bool TeleopMode::operator==(TeleopMode& other)
{
	return name == other.name;
}
