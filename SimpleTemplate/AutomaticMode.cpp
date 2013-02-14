#include "AutomaticMode.h"

AutomaticMode::AutomaticMode(Targeting* t, EasyController* c)
{
	targeting = t;
	controller = c;

	name = "AUTO";
	indicator = "Automatic Target Detection";
}

AutomaticMode::~AutomaticMode()
{

}

void AutomaticMode::begin(DriverStationLCD *screen)
{
	TeleopMode::begin(screen);
	targeting->enable();
}

void AutomaticMode::execute(DriverStationLCD *screen)
{

}

void AutomaticMode::end()
{
	targeting->disable();
}
