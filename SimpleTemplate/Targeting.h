#ifndef TARGETING_H
#define TARGETING_H

#include "Shooter.h"
#include "ShooterTilt.h"
#include "CANDrive.h"
#include "Task.h"
#include "Synchronized.h"
#include "Vision/AxisCamera.h"
#include <unordered_map>

using namespace std;

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

	vector<VisibleTarget*> getVisibleTargets();
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
	
	vector<VisibleTarget*>::const_iterator findGoal(GoalType::id target);
	const static double allowableError = 0.08;

	Shooter *shooter;
	ShooterTilt *tilt;
	CANDrive *drive;
	AxisCamera *camera;

	Task t_watchCamera;

	vector<VisibleTarget*> visibleTargets;
	vector<GoalDetails> goals;

	GoalType::id scoreParticle(BinaryImage* filtered, BinaryImage* threshold, ParticleAnalysisReport* report);
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, GoalType::id goal);
	GoalType::id scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreRectangularity(ParticleAnalysisReport *report);
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report);
};

#endif