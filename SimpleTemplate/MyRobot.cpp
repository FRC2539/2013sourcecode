#include "WPILib.h"
/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */
class RobotDemo: public SimpleRobot {

	Joystick rightStick;
	Joystick leftStick;
	Joystick controller;
	CANJaguar frontWheel;
	CANJaguar backWheel;
	CANJaguar tilt;
	CANJaguar rightDrive;
	CANJaguar leftDrive;
	DigitalInput tiltBottom;
	DigitalInput tiltTop;
	DigitalInput tiltEncoder;
	DigitalInput tiltSlot;
	DoubleSolenoid trigger;
	Compressor compressor;
	Task t_trackTicks;
	Timer timer;
	//RobotDrive myRobot; // robot drive system
	
private:
	
	
public:
	RobotDemo(void) :

		rightStick(1),
		leftStick(2),
		controller(3),
		frontWheel(3),
		backWheel(5),
		tilt(2),
		rightDrive(6),
		leftDrive(4),
		tiltBottom(3),
		tiltTop(2),
		tiltEncoder(4),
		tiltSlot(5),
		trigger(1, 2),// as they are declared above.
		compressor(1, 1),
		t_trackTicks("trackTicks", (FUNCPTR)s_trackTicks)
		//myRobot(leftDrive, rightDrive) // these must be initialized in the same order
	{
		//myRobot.SetExpiration(0.1);
		rightDrive.ConfigEncoderCodesPerRev(360);
		leftDrive.ConfigEncoderCodesPerRev(360);
	}

	/**
	 * Drive left & right motors for 2 seconds then stop
	 */
	void Autonomous(void) {
		compressor.Start();
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void) {
		//myRobot.SetSafetyEnabled(true);
		DriverStationLCD* screen = DriverStationLCD::GetInstance();
		rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		frontWheel.Set(0);
		backWheel.Set(0);
		compressor.Start();
		tickValue = 0;
		tiltMotorValue = 0;
		lastTrue = false;
		
		t_trackTicks.Start();
		
		//**************************************************************
		//***************************MAIN LOOP**************************
		//**************************************************************
		
		while (IsOperatorControl()) {

			
			
			// drive with arcade style using the controller
			arcadeDrive(controller.GetRawAxis(2),controller.GetRawAxis(4));

			//shooter code with printouts
			setShooterSpeeds();
			
			setTiltSpeed();

			//trigger cylinder code
			if (rightStick.GetRawButton(1)) {
				trigger.Set(DoubleSolenoid::kForward);
			} else {
				trigger.Set(DoubleSolenoid::kReverse);
			}
			
			if(controller.GetRawButton(5)){
				rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
				leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
			}else{
				rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
				leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			}
			
			screen->PrintfLine(DriverStationLCD::kUser_Line1, "Front Speed: %f", frontWheel.Get());
			screen->PrintfLine(DriverStationLCD::kUser_Line2, "Back Speed: %f", backWheel.Get());
			screen->PrintfLine(DriverStationLCD::kUser_Line3, "# ticks :%i", tickValue);

			screen->UpdateLCD();
			Wait(0.005); // wait for a motor update time
		}
		
		//**************************************************************
		//************************END MAIN LOOP*************************
		//**************************************************************
		
		t_trackTicks.Stop();
	}

	
	
	/**
	 * Runs during test mode
	 */
	void Test() {

	}
	
	//**************************************************************
	//**********************Begin Drive Code************************
	//**************************************************************
	
	void arcadeDrive(float xAxis, float yAxis){
		float rightPower;
		float leftPower;
		
		rightPower = yAxis;
		leftPower = yAxis;
		
		rightPower -= xAxis;
		leftPower += xAxis;
		
		rightDrive.Set(rightPower);
		leftDrive.Set(leftPower);
	}

	//**************************************************************
	//*********************Begin Shooter Tilt Code******************
	//**************************************************************

	int tickValue;
	int tiltMotorValue;
	bool lastTrue;
	
	
	static int s_trackTicks(RobotDemo* robot) {
		
		robot->trackTicks();
		return 1;
	}
	
	void trackTicks(){
		while (true)
		{
			if(!tiltEncoder.Get() && !lastTrue){
				if(tilt.Get() > 0){
					tickValue++;	
				}else if(tilt.Get() < 0){
					tickValue--;
				}
				lastTrue = true;
			}
			
			if(!tiltEncoder.Get()){
				lastTrue = false;
			}
			
			Wait(.01);
			
		}
	}
	
	void manualTiltControl(){
		//manual shooter tilt control
		if (rightStick.GetRawButton(3)) {
			if(leftStick.GetRawButton(4) && !tiltSlot.Get()){
				tiltMotorValue = 0;
			}else{
				tiltMotorValue = 1;
			}
		} else if (rightStick.GetRawButton(2)) {
			
			if(leftStick.GetRawButton(4) && !tiltSlot.Get()){
				tiltMotorValue = 0;
			}else{
				tiltMotorValue = -1;
			}
			
		} else {
			tiltMotorValue = 0;
		}
	}
	
	void tiltToPoint(int point){
		if(tickValue > point){
			tiltMotorValue = 1;
		}else if(tickValue < point){
			tiltMotorValue = -1;
		}else{
			tiltMotorValue = 0;
		}
	}
	
	
	
	void setTiltSpeed(){
		if(rightStick.GetRawButton(3)||rightStick.GetRawButton(2)){
			manualTiltControl();
		}else if(rightStick.GetRawButton(4)){
			//tiltToPoint(0);
		}else{
			tiltMotorValue = 0;
		}
		
		if(!tiltBottom.Get() && tiltMotorValue > 0){
			tiltMotorValue = 0;
		}
		if(!tiltTop.Get() && tiltMotorValue < 0){
			tiltMotorValue = 0;
		}
		
		tilt.Set(tiltMotorValue);
	
	}
	
	//**************************************************************
	//**********************end shooter tilt code*******************
	//**************************************************************
	
	
	//**************************************************************
	//***********************start shooter code*********************
	//**************************************************************
	
	float frontShooterSpeed;
	float backShooterSpeed;
	
	void determineShooterSpeeds(){
		
		
		if(leftStick.GetRawButton(3)){
			frontShooterSpeed =-.65;
			backShooterSpeed = -.57;
		}else{
			if((-1 * (rightStick.GetThrottle() * -1 + 1) / 2) < 0){
				frontShooterSpeed = ((-1 * (rightStick.GetThrottle() * -1 + 1) / 2) - .08);
			}else{
				frontShooterSpeed = 0;
			}
			//sprintf(message, "front speed: %f", frontWheel.Get() * -1);
			//screen->PrintfLine(DriverStationLCD::kUser_Line1, message);

			backShooterSpeed = (-1 * (rightStick.GetThrottle() * -1 + 1) / 2);
			//sprintf(message, "back speed: %f", backWheel.Get() * -1);
			//screen->PrintfLine(DriverStationLCD::kUser_Line2, message);
		}
	}
	
	void setShooterSpeeds(){
		determineShooterSpeeds();
		frontWheel.Set(frontShooterSpeed);
		backWheel.Set(backShooterSpeed);
	}
	


	//**************************************************************
	//**********************End Shooter Code************************
	//**************************************************************
	
};

START_ROBOT_CLASS(RobotDemo)
;

