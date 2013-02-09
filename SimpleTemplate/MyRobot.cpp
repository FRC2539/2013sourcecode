#include "WPILib.h"
#include "ShooterTilt.h"
/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */ 
class RobotDemo : public SimpleRobot
{
	
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
	RobotDrive myRobot; // robot drive system
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
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void)
	{
		compressor.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void)
	{
		myRobot.SetSafetyEnabled(true);
		DriverStationLCD* screen = DriverStationLCD::GetInstance();
		char message[50];
		rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		compressor.Start();
		frontWheel.Set(0);
		backWheel.Set(0);
		
		while (IsOperatorControl())
		{
			
			// drive with arcade style using the controller
			myRobot.ArcadeDrive(controller.GetY(),controller.GetX()); 
			
			
			//
			//shooter code with printouts
			//
			frontWheel.Set(-1 * (rightStick.GetThrottle() * -1 + 1) / 2);
			sprintf(message,"front speed: %f", frontWheel.Get()*-1);
			screen->PrintfLine(DriverStationLCD::kUser_Line1, message);
			
			backWheel.Set(-1 * (leftStick.GetThrottle() * -1 + 1) / 2);
			sprintf(message,"back speed: %f", backWheel.Get()*-1);
			screen->PrintfLine(DriverStationLCD::kUser_Line2, message);
						
			//manual shooter tilt control
			if(rightStick.GetRawButton(3)){
				tilt.Set(1);
			}else if(rightStick.GetRawButton(2)){
				tilt.Set(-1);
			}else{
				tilt.Set(0);
			}
			
			//trigger cylinder code
			if(rightStick.GetRawButton(1)){
				trigger.Set(DoubleSolenoid::kForward);
			}else{
				trigger.Set(DoubleSolenoid::kReverse);
			}
			
			screen->UpdateLCD();
			Wait(0.005);				// wait for a motor update time
		}
	}
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
	
	
};

START_ROBOT_CLASS(RobotDemo);


