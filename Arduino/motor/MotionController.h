#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include "Arduino.h"
#include <Constants.h>
#include <PID_v1.h>
#include <DualVNH5019MotorShield.h>

class MotionController {
public:
    MotionController();
    bool executeCommand(String command);
    void initPid();
    static void M1CountInc();
    static void M2CountInc();
    void moveBackwardGrids(long grids);
    void moveForwardGrids(long grids);
    void moveTicks(long ticks, bool isForward);
    void turn(bool isClockwise);

private:
    static const double kp = 0.5, ki = 0, kd = 0;
    static volatile long M1Count, M2Count;
    static const int setSpeed = 250;
    DualVNH5019MotorShield motorShield;
    double SetpointLeft, InputLeft, OutputLeft;
    double SetpointRight, InputRight, OutputRight;
    PID* pidLeft;
    PID* pidRight;
    void updatePid();
    void printInOut();
    void printCounts();
};

#endif