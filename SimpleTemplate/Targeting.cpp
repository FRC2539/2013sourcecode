#include "Targeting.h"
#include "Vision/RGBImage.h"
#include "Vision/HSLImage.h"
#include "Vision/BinaryImage.h"
#include <cmath>
#include <vector>

using namespace std;

//Camera constants used for distance calculation
#define X_IMAGE_RES 640
#define VIEW_ANGLE 43.5
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
const double xMax[XMAXSIZE] = {1, 1, 1, 1, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, 1, 1, 1, 1};
const double xMin[XMINSIZE] = {.4, .6, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, .1, 0.6, 0};
const double yMax[YMAXSIZE] = {1, 1, 1, 1, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, .5, 1, 1, 1, 1};
const double yMin[YMINSIZE] = {
	.4, .6, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,
	.05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05, .05,
	.05, .05, .6, 0
};

Targeting::Targeting(Shooter* s, ShooterTilt* t, CANDrive* d, const char* cameraIP):
	t_watchCamera("watchCamera", (FUNCPTR)s_watchCamera)
{
	shooter = s;
	tilt = t;
	drive = d;

	goals[Targeting::topGoal] = Goal("High Goal", 62, 20);
	goals[Targeting::middleGoal] = Goal("Middle Goal", 62, 29);
	goals[Targeting::lowGoal] = Goal("Low Goal", 37, 32);

	camera = &AxisCamera::GetInstance(cameraIP);
	camera->WriteResolution(AxisCamera::kResolution_640x480);
	camera->WriteBrightness(0);
}

Targeting::~Targeting()
{
	clearTargets();
}

void Targeting::fire(Targeting::goal target)
{
    disable();
    
    unordered_map<Targeting::goal, Target>::const_iterator goalIterator;
    Target goal;
    
    getFreshTargets();
    while (true)
    {
        goalIterator = visibleTargets.find(target);
        if (goalIterator == visibleTargets.end())
        {
            return;
        }
        
        Target goal = goalIterator->second;
        
        if (abs(goal.x) < .05)
        {
            break;
        }
        
        float rotation = calculateRotation(goal.distance, goal.x);
        drive->rotate(rotation);
        
        getFreshTargets();
    }
    
    ShooterSettings settings = calculateShooterSettings(goal.distance);
    
    shooter->setSpeed(settings.speed);
    tilt->goToPosition(settings.position);
    
    shooter->fire();
    
    enable();
}

unordered_map<Targeting::goal, Target*> Targeting::getVisibleTargets()
{
	return visibleTargets;
}

Targeting::Goal Targeting::getGoal(Targeting::goal goal)
{
    return goals[goal];
}


void Targeting::enable()
{
	t_watchCamera.Start();
}

void Targeting::disable()
{
	t_watchCamera.Stop();
}

void Targeting::clearTargets()
{
	for (auto i = visibleTargets.begin(); i != visibleTargets.end(); i++)
	{
		delete i->second;
	}
}

int Targeting::s_watchCamera(Targeting* Targeting)
{
	Targeting->watchCamera();
	return 0;
}

void Targeting::watchCamera()
{
	Threshold threshold(100, 180, 200, 255, 200, 255); //HSV threshold criteria, ranges are in that order
	ParticleFilterCriteria2 criteria[] = {
			{IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
	};
    
    float center;

	SEM_ID freshImage;
	while (true)
	{
		freshImage = camera->GetNewImageSem();
		semTake(freshImage, WAIT_FOREVER);
        
        clearTargets();
		findTargets(camera->GetImage(), threshold, criteria);
        
        if (visibleTargets.size() > 0)
        {            
            center = 0;
            for (auto i = visibleTargets.begin(); i != visibleTargets.end(); i++)
            {
                center += i->second.y;
            }
            
            center /= visibleTargets.size();
            
            if (center > 0.1)
            {
                tilt->changePosition(1);
            }
            else if (center < 0.1)
            {
                tilt->changePosition(-1);
            }
        }

		semGive(freshImage);
	}
}

void Targeting::findTargets(HSLImage *image, Threshold& threshold, ParticleFilterCriteria2& criteria)
{
	BinaryImage *thresholdImage = image->ThresholdHSV(threshold);  // get just the green target pixels
	//thresholdImage->Write("/threshold.bmp");

	BinaryImage *convexHullImage = thresholdImage->ConvexHull(false);  // fill in partial and full rectangles
	//convexHullImage->Write("/ConvexHull.bmp");

	// This code always results in a blank image. Find out why...
	//BinaryImage *filteredImage = convexHullImage->ParticleFilter(criteria, 1);	//Remove small particles
	//filteredImage->Write("Filtered.bmp");
	BinaryImage *filteredImage = convexHullImage;

	vector<ParticleAnalysisReport> *reports = filteredImage->GetOrderedParticleAnalysisReports();  //get a particle analysis report for each particle
    double distance;

	//Iterate through each particle, scoring it and determining whether it is a target or not
	for (unsigned i = 0; i < reports->size(); i++) {

		ParticleAnalysisReport *report = &(reports->at(i));
        
        Targeting::goal current = scoreParticle(filteredImage, thresholdImage, report);
        
        if (current != Targeting::none)
        {
            distance = computeDistance(thresholdImage, report, current);
            visibleTargets.push_back(new Target(
                distance, 
                current, 
                report->center_mass_x_normalized,
                report->center_mass_y_normalized
            ));
        }
	}

	// be sure to delete images after using them
	delete filteredImage;
	delete convexHullImage;
	delete thresholdImage;
	delete image;

	//delete allocated reports also
	delete reports;
}

/**
	* Computes the estimated distance to a target using the height of the particle in the image. For more information and graphics
	* showing the math behind this approach see the Vision Processing section of the ScreenStepsLive documentation.
	*
	* @param image The image to use for measuring the particle estimated rectangle
	* @param report The Particle Analysis Report for the particle
	* @param outer True if the particle should be treated as an outer target, false to treat it as a center target
	* @return The estimated distance to the target in Inches.
	*/
double Targeting::computeDistance (BinaryImage* image, ParticleAnalysisReport* report, Targeting::goal goal) {
	double rectShort, height;
	int targetHeight;

	imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);
	//using the smaller of the estimated rectangle short side and the bounding rectangle height results in better performance
	//on skewed rectangles
	height = min(report->boundingRect.height, rectShort);
	targetHeight = goals[goal].height;

	return X_IMAGE_RES * targetHeight / (height * 12 * 2 * tan(VIEW_ANGLE*PI/(180*2)));
}

/**
	* Computes a score (0-100) comparing the aspect ratio to the ideal aspect ratio for the target. This method uses
	* the equivalent rectangle sides to determine aspect ratio as it performs better as the target gets skewed by moving
	* to the left or right. The equivalent rectangle is the rectangle with sides x and y where particle area= x*y
	* and particle perimeter= 2x+2y
	*
	* @param image The image containing the particle to score, needed to perform additional measurements
	* @param report The Particle Analysis Report for the particle, used for the width, height, and particle number
	* @return The aspect ratio score (0-100)
	*/
Targeting::goal Targeting::scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report){
	double rectLong, rectShort, score;

	imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_LONG_SIDE, &rectLong);
	imaqMeasureParticle(image->GetImaqImage(), report->particleIndex, 0, IMAQ_MT_EQUIVALENT_RECT_SHORT_SIDE, &rectShort);

    
    for (auto i = goals.begin(); i != goals.end(); i++)
    {
        score = 100 * (1-fabs((1-((rectLong/rectShort)/i->second.aspectRatio))));
        
        if (score > ASPECT_RATIO_LIMIT)
        {
            return i->first;
        }
    }
    
    return Targeting::none;
}

/**
	* Computes a score (0-100) estimating how rectangular the particle is by comparing the area of the particle
	* to the area of the bounding box surrounding it. A perfect rectangle would cover the entire bounding box.
	*
	* @param report The Particle Analysis Report for the particle to score
	* @return The rectangularity score (0-100)
	*/
double Targeting::scoreRectangularity(ParticleAnalysisReport *report){
	if(report->boundingRect.width*report->boundingRect.height !=0){
		return 100*report->particleArea/(report->boundingRect.width*report->boundingRect.height);
	} else {
		return 0;
	}
}

/**
	* Computes a score based on the match between a template profile and the particle profile in the X direction. This method uses the
	* the column averages and the profile defined at the top of the sample to look for the solid vertical edges with
	* a hollow center.
	*
	* @param image The image to use, should be the image before the convex hull is performed
	* @param report The Particle Analysis Report for the particle
	*
	* @return The X Edge Score (0-100)
	*/
double Targeting::scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report){
	double total = 0;
	LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_COLUMN_AVERAGES, report->boundingRect);
	for(int i=0; i < (averages->columnCount); i++){
		if(xMin[i*(XMINSIZE-1)/averages->columnCount] < averages->columnAverages[i]
			&& averages->columnAverages[i] < xMax[i*(XMAXSIZE-1)/averages->columnCount]){
			total++;
		}
	}
	total = 100*total/(averages->columnCount);	  //convert to score 0-100
	imaqDispose(averages);						  //let IMAQ dispose of the averages struct
	return total;
}

/**
	* Computes a score based on the match between a template profile and the particle profile in the Y direction. This method uses the
	* the row averages and the profile defined at the top of the sample to look for the solid horizontal edges with
	* a hollow center
	*
	* @param image The image to use, should be the image before the convex hull is performed
	* @param report The Particle Analysis Report for the particle
	*
	* @return The Y Edge score (0-100)
	*/
double Targeting::scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report){
	double total = 0;
	LinearAverages *averages = imaqLinearAverages2(image->GetImaqImage(), IMAQ_ROW_AVERAGES, report->boundingRect);
	for(int i=0; i < (averages->rowCount); i++){
		if(yMin[i*(YMINSIZE-1)/averages->rowCount] < averages->rowAverages[i]
			&& averages->rowAverages[i] < yMax[i*(YMAXSIZE-1)/averages->rowCount]){
			total++;
		}
	}
	total = 100*total/(averages->rowCount);	 //convert to score 0-100
	imaqDispose(averages);					  //let IMAQ dispose of the averages struct
	return total;
}

Targeting::goal Targeting::scoreParticle(BinaryImage* filled, BinaryImage* original, ParticleAnalysisReport* report)
{
    double rectangularity = scoreRectangularity(report);
    
    if (rectangularity < RECTANGULARITY_LIMIT)
    {
        return Targeting::none;
    }
    
    double xEdge = scoreXEdge(original, report);
    if (xEdge < X_EDGE_LIMIT)
    {
        return Targeting::none;
    }
    
    double yEdge = scoreYEdge(original, report);
    if (yEdge < X_EDGE_LIMIT)
    {
        return Targeting::none;
    }
    
    return scoreAspectRatio(filled, report);
}

void Targeting::getFreshTargets()
{
    while ( ! camera->IsFreshImage())
    {
        Wait(0.05);
    }
    
    Threshold threshold(100, 180, 200, 255, 200, 255); //HSV threshold criteria, ranges are in that order
    ParticleFilterCriteria2 criteria[] = {
            {IMAQ_MT_AREA, AREA_MINIMUM, 65535, false, false}
    };
        
    clearTargets();
    findTargets(camera->GetImage(), threshold, criteria);    
}

ShooterSettings Targeting::calculateShooterSettings(double distance)
{
    ShooterSettings settings;
    
    settings.speed = .65;
    settings.position = 20;
}

void Targeting::calculateRotation(double distance, float x)
{
    return x * 20;
}

