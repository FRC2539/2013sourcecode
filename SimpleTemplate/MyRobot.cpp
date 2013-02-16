#include "WPILib.h"
#include "Vision/RGBImage.h"
#include "Vision/HSLImage.h"
#include "Vision/BinaryImage.h"
#include "Math.h"

/**
 * This is a demo program showing the use of the RobotBase class.
 * The SimpleRobot class is the base of a robot application that will automatically call your
 * Autonomous and OperatorControl methods at the right time as controlled by the switches on
 * the driver station or the field controls.
 */

//*****************Start tracking constants*****************

//Camera constants used for distance calculation
#define X_IMAGE_RES 320		//X Image resolution in pixels, should be 160, 320 or 640
//#define VIEW_ANGLE 48		//Axis 206 camera
#define VIEW_ANGLE 43.5  //Axis M1011 camera
#define PI 3.141592653

//Score limits used for target identification
#define RECTANGULARITY_LIMIT 60
#define ASPECT_RATIO_LIMIT 75
#define X_EDGE_LIMIT 40
#define Y_EDGE_LIMIT 60

//Minimum area of particles to be considered
#define AREA_MINIMUM 500

//Edge profile constants used for hollowness score calculation
#define XMAXSIZE 24
#define XMINSIZE 24
#define YMAXSIZE 24
#define YMINSIZE 48
const double xMax[XMAXSIZE] = { 1, 1, 1, 1, .5, .5, .5, .5, .5, .5, .5, .5, .5,
		.5, .5, .5, .5, .5, .5, .5, 1, 1, 1, 1 };
const double xMin[XMINSIZE] = { .4, .6, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1,
		.1, .1, .1, .1, .1, .1, .1, .1, .1, .1, 0.6, 0 };
const double yMax[YMAXSIZE] = { 1, 1, 1, 1, .5, .5, .5, .5, .5, .5, .5, .5, .5,
		.5, .5, .5, .5, .5, .5, .5, 1, 1, 1, 1 };
const double yMin[YMINSIZE] = { .4, .6, .05, .05, .05, .05, .05, .05, .05, .05,
		.05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,
		.05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,
		.05, .05, .05, .05, .05, .05, .05, .05, .6, 0 };

// ***********************end tracking constants**********************


class RobotDemo: public SimpleRobot {

	//Structure to represent the scores for the various tests used for target identification
	struct Scores {
		double rectangularity;
		double aspectRatioInner;
		double aspectRatioOuter;
		double xEdge;
		double yEdge;
	};

	enum Goal {
		top, middle, bottom, unknown
	};

	struct GoalPosition {
		double x;
		double distance;
		Goal goal;
	};

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
		t_trackTicks("trackTicks", (FUNCPTR) s_trackTicks)
		
				
	//myRobot(leftDrive, rightDrive) // these must be initialized in the same order
	{
		//myRobot.SetExpiration(0.1);
		rightDrive.ConfigEncoderCodesPerRev(360);
		leftDrive.ConfigEncoderCodesPerRev(360);
	}

	/**
	 * autonomous code
	 */
	void Autonomous(void) {
		
		DriverStation *station = DriverStation::GetInstance();
		DriverStationLCD *screen = DriverStationLCD::GetInstance();
		clearLCD(screen);
		Wait(.1);
		
		compressor.Start();
		
		
		if(station->GetDigitalIn(1)){
			frontWheel.Set(-.64);
			backWheel.Set(-.56);
		
			Wait(.05);
		
			screen->PrintfLine(DriverStationLCD::kUser_Line1, "front:%d",frontWheel.Get());
			screen->PrintfLine(DriverStationLCD::kUser_Line2, "back :%d",backWheel.Get());
			screen->UpdateLCD();
		
			tickValue = 0;
			
			while(tickValue < 6){
				trackTicks();
				tiltToPoint(6);
			}
		
			Wait(.3);
			
			//loop for shooting 3 frisbees
			for(int i = 0; i < 3; i++){
				Wait(.9);
				
				trigger.Set(DoubleSolenoid::kForward);
		
				Wait(.9);
		
				trigger.Set(DoubleSolenoid::kReverse);
			}
			
			Wait(1);
			
			frontWheel.Set(0);
			backWheel.Set(0);
			
			//
			if(station->GetDigitalIn(2)){
				arcadeDrive(0,.6);
				Wait(.4);
				
				arcadeDrive(0,0);
				
			}
			if(station->GetDigitalIn(3)){
				arcadeDrive(0,-.6);
				Wait(.4);
				
				arcadeDrive(0,0);
			}
		}
		
	}

	/**
	 * Runs the motors with arcade steering. 
	 */
	void OperatorControl(void) {
		//myRobot.SetSafetyEnabled(true);
		DriverStationLCD* screen = DriverStationLCD::GetInstance();
		rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
		//frontWheel.Set(0);
		//backWheel.Set(0);
		compressor.Start();
		tickValue = 0;
		tiltMotorValue = 0;
		lastTrue = false;
		//t_trackTicks.Start();

		GoalPosition highestGoal;
		
		//**************************************************************
		//***************************MAIN LOOP**************************
		//**************************************************************

		while (IsOperatorControl()) {

			trackTicks();
			clearLCD(screen);
			// drive with arcade style using the controller
			arcadeDrive(controller.GetRawAxis(2), controller.GetRawAxis(4));

			//shooter code with printouts
			setShooterSpeeds();

			setTiltSpeed();

			//trigger cylinder code
			if (rightStick.GetRawButton(1)) {
				trigger.Set(DoubleSolenoid::kForward);
			} else {
				trigger.Set(DoubleSolenoid::kReverse);
			}

			if (controller.GetRawButton(5)) {
				rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
				leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Coast);
			} else {
				rightDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
				leftDrive.ConfigNeutralMode(CANJaguar::kNeutralMode_Brake);
			}
			
			
			if(leftStick.GetRawButton(2)){
				highestGoal = analyzeImage(screen);
			}
			if(leftStick.GetRawButton(1)){
				tiltToPoint(2);
			}
			
			screen->PrintfLine(DriverStationLCD::kUser_Line1,
					"Front Speed: %f", frontWheel.Get());
			screen->PrintfLine(DriverStationLCD::kUser_Line2, "Back Speed: %f",
					backWheel.Get());
			screen->PrintfLine(DriverStationLCD::kUser_Line3, "# ticks :%s",
					tiltEncoder.Get() ? "On":"Off");
			screen->PrintfLine(DriverStationLCD::kUser_Line4, "distance %d",
					highestGoal.distance);
			screen->PrintfLine(DriverStationLCD::kUser_Line5, "xValue %d",
					highestGoal.x);
			screen->PrintfLine(DriverStationLCD::kUser_Line6, "#ticks :%i",\
					tickValue);

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

	void arcadeDrive(float xAxis, float yAxis) {
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

	void trackTicks() {
			if (!tiltEncoder.Get() && !lastTrue) {
				if (tilt.Get() > 0) {
					tickValue++;
				} else if (tilt.Get() < 0) {
					tickValue--;
				}
				lastTrue = true;
			}

			if (tiltEncoder.Get()) {
				lastTrue = false;
			}
			
			if(!tiltTop.Get()){
				tickValue = 0;
			}

			Wait(.01);
	}

	void manualTiltControl() {
		//manual shooter tilt control
		if (rightStick.GetRawButton(3)) {
			if (leftStick.GetRawButton(4) && !tiltSlot.Get()) {
				tiltMotorValue = 0;
			} else {
				tiltMotorValue = 1;
			}
		} else if (rightStick.GetRawButton(2)) {

			if (leftStick.GetRawButton(4) && !tiltSlot.Get()) {
				tiltMotorValue = 0;
			} else {
				tiltMotorValue = -1;
			}

		} else {
			tiltMotorValue = 0;
		}
	}

	void tiltToPoint(int point) {
		if (tickValue > point) {
			tilt.Set(-1);
		} else if (tickValue < point) {
			tilt.Set(1);
		} else {
			tilt.Set(0);
		}
	}

	void setTiltSpeed() {
		if (rightStick.GetRawButton(3) || rightStick.GetRawButton(2)) {
			manualTiltControl();
		} else if (rightStick.GetRawButton(4)) {
			//tiltToPoint(0);
		} else {
			tiltMotorValue = 0;
		}

		if (!tiltBottom.Get() && tiltMotorValue > 0) {
			tiltMotorValue = 0;
		}
		if (!tiltTop.Get() && tiltMotorValue < 0) {
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

	void determineShooterSpeeds() {

		if (leftStick.GetRawButton(3)) {
			frontShooterSpeed = -.68;
			backShooterSpeed = -.6;
		} else {
			if ((-1 * (leftStick.GetThrottle() * -1 + 1) / 2) < 0) {
				frontShooterSpeed = ((-1 * (leftStick.GetThrottle() * -1 + 1)
						/ 2) - .08);
			} else {
				frontShooterSpeed = 0;
			}
			//sprintf(message, "front speed: %f", frontWheel.Get() * -1);
			//screen->PrintfLine(DriverStationLCD::kUser_Line1, message);

			backShooterSpeed = (-1 * (leftStick.GetThrottle() * -1 + 1) / 2);
			//sprintf(message, "back speed: %f", backWheel.Get() * -1);
			//screen->PrintfLine(DriverStationLCD::kUser_Line2, message);
		}
	}

	void setShooterSpeeds() {
		determineShooterSpeeds();
		frontWheel.Set(frontShooterSpeed);
		backWheel.Set(backShooterSpeed);
	}

	//**************************************************************
	//**********************End Shooter Code************************
	//**************************************************************


	//**************************************************************
	//*********************Start tracking code**********************
	//**************************************************************


	GoalPosition analyzeImage(DriverStationLCD* screen) {

		timer.Reset();
		timer.Start();
		
		clearLCD(screen);
		AxisCamera &camera = AxisCamera::GetInstance("10.25.39.11");  //To use the Axis camera uncomment this line
		
		Scores* scores;
		Threshold threshold(100, 180, 200, 255, 200, 255);
		GoalPosition position;
		HSLImage *image;

		image = camera.GetImage();

		if ((image == (void *) 0) || image->GetWidth() == 0
				|| image->GetHeight() == 0) {
			screen->PrintfLine(DriverStationLCD::kUser_Line1,
					"image is not valid");
			screen->UpdateLCD();
			
			return position;
		}

		BinaryImage *thresholdImage;
		
		try{
			thresholdImage = image->ThresholdHSV(threshold);
		}catch(int e){
			return position;
		}
		// filter out all but the green pixels
		screen->PrintfLine(DriverStationLCD::kUser_Line1,
				"threshold");
		screen->UpdateLCD();

		BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);
		//fill in all rectangles and partial rectangles
		screen->PrintfLine(DriverStationLCD::kUser_Line2,
				"convex hull");

		// This code always results in a blank image. Find out why...
		//BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);	//Remove small particles
		//filteredImage->Write("Filtered.bmp");
		BinaryImage *filteredImage = convexHullImage;

		//get a particle analysis report for each particle
		vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();
		scores = new Scores[reports->size()];

		for (unsigned i = 0; i < reports->size(); i++) {

			ParticleAnalysisReport *report = &(reports->at(i));

			scores[i].rectangularity = scoreRectangularity(report);
			scores[i].aspectRatioOuter = scoreAspectRatio(filteredImage,
					report, true);
			scores[i].aspectRatioInner = scoreAspectRatio(filteredImage,
					report, false);
			scores[i].xEdge = scoreXEdge(thresholdImage, report);
			scores[i].yEdge = scoreYEdge(thresholdImage, report);

			

			if (scoreCompare(scores[i], false)) {

				position.x = report->center_mass_x_normalized;
				position.distance = computeDistance(thresholdImage, report,
						false);
				position.goal = top;
				//printf("particle: %d  is a High Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
				//printf("Distance: %f \n", computeDistance(thresholdImage, report, false));
				screen->PrintfLine(DriverStationLCD::kUser_Line3,
						"** High Goal Found **");
				screen->PrintfLine(DriverStationLCD::kUser_Line6,
						"Time to process: %f", timer.Get());
				screen->UpdateLCD();
				return position;

			} else if (scoreCompare(scores[i], true)) {

				position.x = report->center_mass_x_normalized;
				position.distance = computeDistance(thresholdImage, report,
						false);
				position.goal = middle;
				//printf("particle: %d  is a Middle Goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
				//printf("Distance: %f \n", computeDistance(thresholdImage, report, true));
				screen->PrintfLine(DriverStationLCD::kUser_Line4,
						"* Middle Goal Found *");
				screen->PrintfLine(DriverStationLCD::kUser_Line6,
						"Time to process: %f", timer.Get());
				screen->UpdateLCD();
				return position;

			} else {

				position.x = report->center_mass_x_normalized;
				position.distance = computeDistance(thresholdImage, report,
						false);
				position.goal = unknown;
				//printf("particle: %d  is not a goal  centerX: %f  centerY: %f \n", i, report->center_mass_x_normalized, report->center_mass_y_normalized);
				screen->PrintfLine(DriverStationLCD::kUser_Line5,
						"!! Unknown Rectangle Found !!");
				screen->PrintfLine(DriverStationLCD::kUser_Line6,
						"Time to process: %f", timer.Get());
				screen->UpdateLCD();
				return position;
			}
		}
		screen->PrintfLine(DriverStationLCD::kUser_Line6,"no targets");
		screen->UpdateLCD();
		return position;
	}
	
	//*********************supporting image functions*********************

	bool scoreCompare(Scores scores, bool outer) {
		bool isTarget = true;
		isTarget &= scores.rectangularity > RECTANGULARITY_LIMIT;
		if (outer) {
			isTarget &= scores.aspectRatioOuter > ASPECT_RATIO_LIMIT;
		} else {
			isTarget &= scores.aspectRatioInner > ASPECT_RATIO_LIMIT;
		}
		isTarget &= scores.xEdge > X_EDGE_LIMIT;
		isTarget &= scores.yEdge > Y_EDGE_LIMIT;

		return isTarget;
	}
	
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report){
		double total = 0;
		LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_COLUMN_AVERAGES, report->boundingRect);
		for(int i=0; i < (averages->columnCount); i++){
			if(xMin[i*(XMINSIZE-1)/averages->columnCount] < averages->columnAverages[i] && averages->columnAverages[i] < xMax[i*(XMAXSIZE-1)/averages->columnCount]){
				total++;
			}
		}
		total = 100*total/(averages->columnCount);    //convert to score 0-100
		imaqDispose(averages);              //let IMAQ dispose of the averages struct
		return total;
	}
	
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report){
		double total = 0;
		LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_ROW_AVERAGES, report->boundingRect);
		for(int i=0; i < (averages->rowCount); i++){
			if(yMin[i*(YMINSIZE-1)/averages->rowCount] < averages->rowAverages[i] 
	                                                                   && averages->rowAverages[i] < yMax[i*(YMAXSIZE-1)/averages->rowCount]){
				total++;
			}
		}
		total = 100*total/(averages->rowCount);    //convert to score 0-100
		imaqDispose(averages);            //let IMAQ dispose of the averages struct
		return total;
	} 
	
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, bool outer) {
		double rectShort, height;
		int targetHeight;
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
		//using the smaller of the estimated rectangle short side and the bounding rectangle height results in better performance
		//on skewed rectangles
		height = min(report->boundingRect.height, rectShort);
		targetHeight = outer ? 29 : 21;
	  
		return X_IMAGE_RES * targetHeight / (height * 12 * 2 * tan(VIEW_ANGLE*PI/(180*2)));
	}
	
	double scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report, bool outer){
		double rectLong, rectShort, idealAspectRatio, aspectRatio;
		idealAspectRatio = outer ? (62/29) : (62/20);  //Dimensions of goal opening + 4 inches on all 4 sides for reflective tape
	   
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_LONG_SIDE, &rectLong);
		imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
		//Divide width by height to measure aspect ratio
		if(report->boundingRect.width > report->boundingRect.height){
			
			//particle is wider than it is tall, divide long by short
			aspectRatio = 100*(1-fabs((1-((rectLong/rectShort)/idealAspectRatio))));
			
		} else {
			
			//particle is taller than it is wide, divide short by long
			aspectRatio = 100*(1-fabs((1-((rectShort/rectLong)/idealAspectRatio))));
			
		}
	return (max(0, min(aspectRatio, 100)));    //force to be in range 0-100
	}
	
	double scoreRectangularity(ParticleAnalysisReport *report){
		if(report->boundingRect.width*report->boundingRect.height !=0){
			return 100*report->particleArea/(report->boundingRect.width*report->boundingRect.height);
		} else {
			return 0;
		}  
	}

	//********************end supporting vision code*****************************
	
	void clearLCD(DriverStationLCD* screen){
		screen->PrintfLine(DriverStationLCD::kUser_Line1, "");
		screen->PrintfLine(DriverStationLCD::kUser_Line2, "");
		screen->PrintfLine(DriverStationLCD::kUser_Line3, "");
		screen->PrintfLine(DriverStationLCD::kUser_Line4, "");
		screen->PrintfLine(DriverStationLCD::kUser_Line5, "");
		screen->PrintfLine(DriverStationLCD::kUser_Line6, "");
	}
	
};

START_ROBOT_CLASS(RobotDemo)
;

