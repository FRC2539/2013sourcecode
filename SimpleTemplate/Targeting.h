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

struct Target {
	double distance;
	Targeting::goal goal;

	Target(double d, Targeting::goal g)
	{
		distance = d;
		goal = g;
	}
};

class Targeting {
public:

	Targeting(Shooter* shooter, ShooterTilt* shooterTilt, CANDrive* drive, const char* cameraIP);
	~Targeting();

	void enable();
	void disable();

	void fire(goal target);

	unordered_map<Targeting::goal, Target*> getVisibleTargets();

	typedef enum {topGoal, middleGoal, lowGoal, towerGoal, none} goal;

protected:

	static int s_watchCamera(Targeting* Targeting);
	void watchCamera();

	void findTargets(HSLImage* image);
	void clearTargets();

	Shooter *shooter;
	ShooterTilt *tilt;
	CANDrive *drive;
	AxisCamera *camera;

	Task t_watchCamera;

	unordered_map<Targeting::goal, Target*> visibleTargets;

	struct Goal {
		int height;
		int width;
		float aspectRatio;

		Goal(int w, int h)
		{
			width = w;
			height = h;

			aspectRatio = static_cast<float>(width) / height;
		}
	};
	unordered_map<Targeting::goal, Goal> goals;

	//Structure to represent the scores for the various tests used for target identification
	struct Scores {
		double rectangularity;
        Targeting::goal goal;
		double xEdge;
		double yEdge;

	};

	Scores *scores;

    Targeting::goal scoreParticle(BinaryImage* filtered, BinaryImage* threshold, ParticleAnalysisReport* report);
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, Targeting::goal goal);
	Targeting::goal scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report);
	bool scoreCompare(Scores scores, bool outer);
	double scoreRectangularity(ParticleAnalysisReport *report);
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report);
};

#endif