#include "WPILib.h"
#include "Shooter.h"
#include "ShooterTilt.h"
#include "CANDrive.h"
#include "EasyController.h"
#include "Targeting.h"
#include "ControllerSetup.cpp"
#include "TeleopMode.h"
#include "AutomaticMode.h"
#include "ManualMode.h"
#include "LoadingMode.h"

using namespace std;

/**
 * A robot to play Ultimate Ascent.
 */ 
class FrisbeeBot : public SimpleRobot
{
public:
	FrisbeeBot(void):

		rightStick(1),
		leftStick(2),
		controller(3),
		frontWheel(5),
		backWheel(3),
		tilt(2),
		rightDrive(6),
		leftDrive(4),
		trigger(1,2),
		compressor(1,1),
		drive(&leftDrive, &rightDrive),
		shooter(&frontWheel, &backWheel, &trigger),
		shooterTilt(&tilt, 3, 2, 4),
		targeting(&shooter, &shooterTilt, &drive, "10.25.39.11"),
		automaticTargetingMode(&targeting, &controller),
		manualTargetingMode(&shooter, &shooterTilt, &rightStick),
		loadingMode(&shooterTilt)
	{
		drive.setNeutral(CANJaguar::kNeutralMode_Brake);
		drive.setMode(CANJaguar::kSpeed);

		tilt.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);

		screen = DriverStationLCD::GetInstance();
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		setupRobot();

		while (IsAutonomous() && IsEnabled())
		{

		}
	}

	void OperatorControl(void)
	{
		setupRobot();

		currentMode = &automaticTargetingMode;
		currentMode->begin(screen);
		
		while (IsOperatorControl() && IsEnabled())
		{
			drive.arcade(
				controller.getAxis(GamePad::leftStickY),
				controller.getAxis(GamePad::rightStickX)
			);

			if (controller.isClicked(GamePad::A))
			{
				changeMode(&loadingMode);
			}
			if (rightStick.isClicked(Sidewinder::rightFront))
			{
				changeMode(&manualTargetingMode);
			}

			currentMode->execute(screen);
			screen->UpdateLCD();
			
			Wait(0.005);
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}

protected:
	EasyController rightStick;
	EasyController leftStick;
	EasyController controller;
	CANJaguar frontWheel;
	CANJaguar backWheel;
	CANJaguar tilt;
	CANJaguar rightDrive;
	CANJaguar leftDrive;
	DoubleSolenoid trigger;
	Compressor compressor;
	CANDrive drive;
	Shooter shooter;
	ShooterTilt shooterTilt;
	Targeting targeting;

	AutomaticMode automaticTargetingMode;
	ManualMode manualTargetingMode;
	LoadingMode loadingMode;

	TeleopMode *currentMode;
	DriverStationLCD *screen;

	void setupRobot()
	{
		compressor.Start();

		shooter.setSpeed(0);

		if (shooterTilt.getPosition() == -1)
		{
			shooterTilt.goHome();
		}
	}

	void changeMode(TeleopMode *newMode)
	{
		// If we're unsetting a mode, fall back to the default (Automatic)
		if (currentMode == newMode)
		{
			newMode = &automaticTargetingMode;
		}

		currentMode->end();
		currentMode = newMode;
		currentMode->begin(screen);
	}
};

START_ROBOT_CLASS(FrisbeeBot);


