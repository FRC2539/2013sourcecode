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
	int goal;
    float x;
    float y;

	Target(double d, int g, float xPosition, float yPosition)
	{
		distance = d;
		goal = g;
        x = xPosition;
        y = yPosition;
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

	typedef enum {topGoal, middleGoal, lowGoal, towerGoal, none} goal;

	void fire(Targetting::goal target);

	unordered_map<Targeting::goal, Target*> getVisibleTargets();
    Goal getGoal(Targeting::goal goal);

    struct Goal {
        char* name;
        int height;
        int width;
        float aspectRatio;

        Goal(char* n, int w, int h)
        {
            name = n;
            
            width = w;
            height = h;

            aspectRatio = static_cast<float>(width) / height;
        }
    };

protected:

	static int s_watchCamera(Targeting* Targeting);
	void watchCamera();

	void findTargets(HSLImage* image);
	void clearTargets();
    
    void getFreshTargets();
    ShooterSettings calculateShooterSettings(double distance);
    void calculateRotation(double distance, float x);

	Shooter *shooter;
	ShooterTilt *tilt;
	CANDrive *drive;
	AxisCamera *camera;

	Task t_watchCamera;

	unordered_map<Targeting::goal, Target*> visibleTargets;
	unordered_map<Targeting::goal, Goal> goals;

    Targeting::goal scoreParticle(BinaryImage* filtered, BinaryImage* threshold, ParticleAnalysisReport* report);
	double computeDistance (BinaryImage *image, ParticleAnalysisReport *report, Targeting::goal goal);
	Targeting::goal scoreAspectRatio(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreRectangularity(ParticleAnalysisReport *report);
	double scoreXEdge(BinaryImage *image, ParticleAnalysisReport *report);
	double scoreYEdge(BinaryImage *image, ParticleAnalysisReport *report);
};

#endif