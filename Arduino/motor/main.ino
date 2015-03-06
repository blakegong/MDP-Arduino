#include "Arduino.h"
#include "Constants.h"
#include "PinChangeInt.h"
#include "FlowController.h"
#include "MotionController.h"
#include "SensorController.h"

FlowController* flowController;
MotionController* motionController;
SensorController* sensorController;

int cycles = 0;

void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(2);

    PCintPort::attachInterrupt(Constants::M1_ENCODER_A, MotionController::M1CountInc, CHANGE);
    PCintPort::attachInterrupt(Constants::M2_ENCODER_A, MotionController::M2CountInc, CHANGE);

    motionController = new MotionController();
    sensorController = new SensorController();

    flowController = new FlowController(motionController, sensorController);
}

void loop() {
    flowController->startFSM();

    // if (sensorController->getIRGrids(Constants::IR_FL) >= 3)
    //     motionController->moveForwardGrids(1);
    // else
    //     motionController->turnLeft();
    cycles++;
    // Serial.println(sensorController->getIRGrids(Constants::IR_SHORT_1));
    // Serial.println(sensorController->getIRShortOptimizedCM(Constants::IR_SHORT_1));
    // Serial.println(sensorController->getUlCM(Constants::UL1_PWM, Constants::UL1_TRIG));
    // motionController->moveForward(576);
    // motionController->turnLeft();
    // motionController->moveForward(576);
    // motionController->turnRight();
    // motionController->moveForward(576);
    // motionController->turnRight();
    // motionController->moveForward(576);
    // motionController->turnRight();
    // motionController->moveForward(576);
    // motionController->turnRight();
    // motionController->moveForward(576);
    // motionController->turnLeft();
    // motionController->moveForward(576);
    // motionController->turnLeft();
    // motionController->moveForward(576);
    // motionController->turnLeft();
    // motionController->moveForward(580);
    // motionController->spinMotor(150);
    // motionController->readCounts();
    // motionController->turnLeft();
    // motionController->turnRight();
    // if (cycles >= 10)
    // while (1);
    delay(100);
}