#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include "Arduino.h"
#include <Constants.h>
#include <PID_v1.h>
#include <DualVNH5019MotorShield.h>
#include <SensorController.h>

class MotionController {
public:
    MotionController(SensorController* sensorController);
    static void MLCountInc();
    static void MRCountInc();
    int calibratePos(int grids, bool isTurn, bool isTimeoutEnable = true);
    void calibrateTurn();
    void calibrateMove(unsigned char grids);
    int getDirection();
    int getPosX();
    int getPosY();
    bool moveBackwardGrids(long grids);
    bool moveForwardGrids(long grids);
    bool moveTicks(long ticks, bool isForward);
    bool fastMoveTicks(long ticks);
    void resetCounts();
    void setPosition(int posX, int posY);
    void turn(bool isClockwise);
    DualVNH5019MotorShield motorShield;

private:
    static const double kp = 0.5, ki = 0.25, kd = 0;
    static volatile long MLCount, MRCount;
    static const int moveSpeed = 200;
    static const int fastMoveSpeed = 350;
    static const int turnSpeed = 300;
    int clockwiseTicks, antiClockwiseTicks;
    int forwardTicks [20];
    int lTarget [4];
    int mTarget [4];
    int rTarget [4];
    
    SensorController* sensorController;
    unsigned char direction;
    int posX, posY;
    double SetpointLeft, InputLeft, OutputLeft;
    double SetpointRight, InputRight, OutputRight;
    PID* pidLeft;
    PID* pidRight;
    void initPid();
    void initTicks();
    void updatePid();
};

#endif