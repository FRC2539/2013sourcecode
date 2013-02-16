#include "AutomaticMode.h"
#include <ios>

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
	unordered_map<Targeting::goal, Target*> targets = targeting->getVisibleTargets();
	
	screen->PrintfLine(DriverStationLCD::kUser_Line2, "");
	screen->PrintfLine(DriverStationLCD::kUser_Line3, "");
	
	Targeting::goal rightBumper = Targeting::none;
	Targeting::goal leftBumper = Targeting::none;
	
	auto i = targets.begin();
	if (i != targets.end())
	{   
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"%s: Right Bumper",
			targeting->getGoal(i->first).name
		);
		
		rightBumper = i->first;
		
		i++;
	}
	
	if (i != targets.end())
	{
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"%s: Left Bumper",
			i->second.name
		);
		
		leftBumper = i->first;
	}
	
	if (controller->isClicked(GamePad::rightBumper))
	{
		targeting->fire(rightBumper);
	}
	
	if (controller->isClicked(GamePad::leftBumper))
	{
		targeting->fire(leftBumper);
	}
}

void AutomaticMode::end()
{
	targeting->disable();
}
