#ifndef TARGETTING_H
#define TARGETTING_H

#include "Shooter.h"
#include "ShooterTilt.h"
#include "RobotDrive.h"
#include "Task.h"
#include "Synchronized.h"
#include <vector>

using namespace std;

struct Target {
    double distance;
    Targetting::goal goal;

    Target(double d, Targetting::goal g)
    {
        distance = d;
        goal = g;
    }
};

class Targetting {
public:

    Targetting(Shooter& shooter, ShooterTilt& tilt, RobotDrive& drive);
    ~Targetting();

    void fire(goal target);

    vector<Target> getVisibleTargets();

    typedef enum {topGoal, middleGoal, lowGoal, towerGoal} goal;

protected:

    static int s_watchCamera(Targetting* targetting);
    void watchCamera();

    Shooter shooter;
    ShooterTilt tilt;
    RobotDrive drive;

    Task t_watchCamera;
};

#endif