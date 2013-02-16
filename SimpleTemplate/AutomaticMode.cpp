#include "AutomaticMode.h"
#include <vector>

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
	vector<VisibleTarget*> targets = targeting->getVisibleTargets();
	
	screen->PrintfLine(DriverStationLCD::kUser_Line2, "");
	screen->PrintfLine(DriverStationLCD::kUser_Line3, "");
	
	GoalType::id rightBumper = GoalType::none;
	GoalType::id leftBumper = GoalType::none;
	
	auto i = targets.begin();
	if (i != targets.end())
	{		   
		rightBumper = i->second->goal;
		
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"%s: Right Bumper",
			targeting->getGoal(rightBumper).name
		);
		
		i++;
	}
	
	if (i != targets.end())
	{
		leftBumper = i->second->goal;
		
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"%s: Left Bumper",
			targeting->getGoal(leftBumper).name
		);
		
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
