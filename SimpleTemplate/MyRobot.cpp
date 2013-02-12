#include "WPILib.h"
#include "Shooter.h"
#include "ShooterTilt.h"
#include <cmath>

using namespace std;

/**
 * A robot to play Ultimate Ascent.
 */ 
class FrisbeeBot : public SimpleRobot
{
public:
	RobotDemo(void):

		rightStick(1),
		leftStick(2),
		controller(3),
		frontWheel(5),
		backWheel(3),
		tilt(2),
		rightDrive(6),
		leftDrive(4),
		trigger(1,2),// as they are declared above.
		compressor(1,1),
		myRobot(leftDrive,rightDrive)	// these must be initialized in the same order
	{
		myRobot.SetExpiration(0.1);

		rightDrive.SetVoltageRampRate(24);
		leftDrive.SetVoltageRampRate(24);

		rightDrive.ConfigEncoderCodesPerRev(360);
		leftDrive.ConfigEncoderCodesPerRev(360);

		rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);

		rightDrive.ChangeControlMode(CANJaguar::kSpeed);
		rightDrive.EnableControl();
		leftDrive.ChangeControlMode(CANJaguar::kSpeed);
		leftDrive.EnableControl();

		tilt.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);

		shooterTilt(tilt, 2, 3, 4);
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

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		setupRobot();

		DriverStationLCD* screen = DriverStationLCD::GetInstance();
		bool triggerPressed = false;
		bool triggerWasPressed = rightStick.GetRawButton(1);

		double oldShooterSpeed = getThrottleSpeed();
		double shooterSpeed = 0;
		
		while (IsOperatorControl() && IsEnabled())
		{
			if (rightStick.GetRawButton(3))
			{
				shooterTilt.changePosition(1);
				tilt.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
			}
			else if (rightStick.GetRawButton(2))
			{
				shooterTilt.changePosition(-1);
				tilt.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
			}
			else
			{
				tilt.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			}
			screen->PrintfLine(DriverStationLCD::kUser_Line4, 'Shooter Position: %i', shooterTilt.getPosition());

			shooterSpeed = getThrottleSpeed();

			if (abs(shooterSpeed - oldShooterSpeed) >= 0.01)
			{
				shooter.setSpeed(shooterSpeed);
			}
			screen->PrintfLine(DriverStationLCD::kUser_Line1, 'Front Wheel: %4.2d%%', shooter.getFrontSpeed());
			screen->PrintfLine(DriverStationLCD::kUser_Line2, 'Back Wheel: %4.2d%%', shooter.getBackSpeed());

			triggerPressed = rightStick.GetRawButton(1);
			if (triggerPressed && ! triggerWasPressed)
			{
				shooter.fire();
			}
			triggerWasPressed = triggerPressed;
			
			// drive with arcade style using the controller
			myRobot.ArcadeDrive(controller.GetY(),controller.GetX()); 
			
			screen->UpdateLCD();
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}

protected:
	Joystick rightStick;
	Joystick leftStick;
	Joystick controller;
	CANJaguar frontWheel;
	CANJaguar backWheel;
	CANJaguar tilt;
	CANJaguar rightDrive;
	CANJaguar leftDrive;
	DoubleSolenoid trigger;
	Compressor compressor;
	RobotDrive myRobot;
	Shooter shooter;
	ShooterTilt shooterTilt;

	void setupRobot()
	{
		myRobot.SetSafetyEnabled(false);

		compressor.Start();

		shooter.setSpeed(0);

		if (shooterTilt.getPosition() == -1)
		{
			shooterTilt.goHome();
		}
	}
	
	double getThrottleSpeed()
	{
		return (rightStick.GetThrottle * -1 + 1) / 2;
	}
};

START_ROBOT_CLASS(FrisbeeBot);


