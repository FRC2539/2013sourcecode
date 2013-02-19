#include "CalibrateSystemsMode.h"
#include <vector>
#include <list>

CalibrateSystemsMode::CalibrateSystemsMode(Shooter* s, ShooterTilt* r, Targeting* t, EasyController* c)
{
	shooter = s;
	tilt = r;
	targeting = t;
	controller = c;

	name = "CALIBRATE";
	indicator = "** ROBOT CALIBRATION **";
}

CalibrateSystemsMode::~CalibrateSystemsMode()
{

}

void CalibrateSystemsMode::begin(DriverStationLCD *screen)
{
	TeleopMode::begin(screen);
	targeting->enable();

	currentSystem = RobotSystem::select;
}

void CalibrateSystemsMode::execute(DriverStationLCD *screen)
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

void CalibrateSystemsMode::end()
{
	targeting->disable();
}

void CalibrateSystemsMode::setSystem(RobotSystem::mode newSystem)
{
	if (newSystem == RobotSystem::select)
	{

	}
	currentSystem = newSystem;
}

