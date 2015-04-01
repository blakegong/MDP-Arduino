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
    int calibratePos(int grids, bool isTurn);
    void calibrateTurn();
    void calibrateMove(unsigned char grids);
    int getPosX();
    int getPosY();
    bool moveBackwardGrids(long grids);
    bool moveForwardGrids(long grids);
    bool moveTicks(long ticks, bool isForward);
    void resetCounts();
    void setPosition(int posX, int posY);
    void turn(bool isClockwise);

private:
    static const double kp = 0.5, ki = 0.25, kd = 0;
    static volatile long MLCount, MRCount;
    static const int setSpeed = 150;
    int clockwiseTicks, antiClockwiseTicks;
    int forwardTicks [15];
    DualVNH5019MotorShield motorShield;
    SensorController* sensorController;
    unsigned char direction; // 0:F 1:R 2:B 3:L
    int posX, posY;
    double SetpointLeft, InputLeft, OutputLeft;
    double SetpointRight, InputRight, OutputRight;
    PID* pidLeft;
    PID* pidRight;
    void initPid();
    void updatePid();
    void printInOut();
    void printCounts();
};

#endif