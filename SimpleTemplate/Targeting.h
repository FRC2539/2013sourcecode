#ifndef TARGETING_H
#define TARGETING_H

#include "Shooter.h"
#include "ShooterTilt.h"
#include "CANDrive.h"
#include "Task.h"
#include "Synchronized.h"
#include "Vision/AxisCamera.h"
#include <vector>
#include <list>

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

namespace GoalType {
	enum id {
		top,
		middle,
		low,
		tower,
		none
	};
};

struct VisibleTarget {
	double distance;
	GoalType::id goal;
	float x;
	float y;

	VisibleTarget(double d, GoalType::id g, float xPosition, float yPosition)
	{
		distance = d;
		goal = g;
		x = xPosition;
		y = yPosition;
	}
};

struct GoalDetails {
	char* name;
	int height;
	int width;
	float aspectRatio;

	GoalDetails(char* n, int w, int h)
	{
		name = n;
		
		width = w;
		height = h;

		aspectRatio = static_cast<float>(width) / height;
	}	
};

struct ShooterSettings {
	double speed;
	int position;
};

class Targeting {
public:

	Targeting(Shooter* shooter, ShooterTilt* shooterTilt, CANDrive* drive, const char* cameraIP);
	~Targeting();

	void enable();
	void disable();

	void fire(GoalType::id target);

	list<VisibleTarget*> getVisibleTargets();
	GoalDetails getGoal(GoalType::id goal);

protected:

	static int s_watchCamera(Targeting* Targeting);
	void watchCamera();

	void findTargets(HSLImage* image);
	void clearTargets();
	
	void getFreshTargets();
	ShooterSettings calculateShooterSettings(GoalType::id target, double distance);
	double calculateRotation(double distance, float x);
	double calculateOptimalX(double distance);
	
	list<VisibleTarget*>::const_iterator findGoal(GoalType::id target);
	const static double allowableError = 0.08;

	Shooter *shooter;
	ShooterTilt *tilt;
	CANDrive *drive;
	AxisCamera *camera;

	Task t_watchCamera;

	list<VisibleTarget*> visibleTargets;
	vector<GoalDetails> goals;

	GoalType::id scoreParticle(BinaryImage* filtered, BinaryImage* threshold, ParticleAnalysisReport* report);
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, GoalType::id goal);
	GoalType::id scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreRectangularity(ParticleAnalysisReport *report);
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report);
	
	Threshold threshold;
	ParticleFilterCriteria2 criteria[1];
};

#endif
