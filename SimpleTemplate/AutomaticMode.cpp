#include "AutomaticMode.h"
#include <vector>
#include <list>

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
	list<VisibleTarget*> targets = targeting->getVisibleTargets();
	
	screen->PrintfLine(DriverStationLCD::kUser_Line2, "");
	screen->PrintfLine(DriverStationLCD::kUser_Line3, "");
	
	GoalType::id rightBumper = GoalType::none;
	GoalType::id leftBumper = GoalType::none;
	
	list<VisibleTarget*>::iterator i = targets.begin();
	list<VisibleTarget*>::iterator end = targets.end();
	if (i != end)
	{		   
		rightBumper = (*i)->goal;
		
		screen->PrintfLine(
			DriverStationLCD::kUser_Line2,
			"%s: Right Bumper",
			targeting->getGoal(rightBumper).name
		);
		
		i++;
	}
	
	if (i != end)
	{
		leftBumper = (*i)->goal;
		
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
