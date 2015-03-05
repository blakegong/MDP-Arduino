#include "Arduino.h"
#include "Constants.h"
#include "PinChangeInt.h"
#include "MotionController.h"
#include "SensorController.h"

MotionController* mc;
SensorController* sc;

int cycles = 0;

void setup()
{
    Serial.begin(9600);

    PCintPort::attachInterrupt(Constants::M1_ENCODER_A, MotionController::M1CountInc, CHANGE);
    PCintPort::attachInterrupt(Constants::M2_ENCODER_A, MotionController::M2CountInc, CHANGE);

    mc = new MotionController();
    sc = new SensorController();
}

void loop() {
    // if (sc->getIRGrids(Constants::IR_FL) >= 3)
    //     mc->moveForwardGrids(1);
    // else
    //     mc->turnLeft();
    cycles++;
    // Serial.println(sc->getIRGrids(Constants::IR_SHORT_1));
    // Serial.println(sc->getIRShortOptimizedCM(Constants::IR_SHORT_1));
    // Serial.println(sc->getUlCM(Constants::UL1_PWM, Constants::UL1_TRIG));
    // mc->moveForward(576);
    // mc->turnLeft();
    // mc->moveForward(576);
    // mc->turnRight();
    // mc->moveForward(576);
    // mc->turnRight();
    // mc->moveForward(576);
    // mc->turnRight();
    // mc->moveForward(576);
    // mc->turnRight();
    // mc->moveForward(576);
    // mc->turnLeft();
    // mc->moveForward(576);
    // mc->turnLeft();
    // mc->moveForward(576);
    // mc->turnLeft();
    // mc->moveForward(580);
    // mc->spinMotor(150);
    // mc->readCounts();
    // mc->turnLeft();
    // mc->turnRight();
    // if (cycles >= 10)
    // while (1);
    delay(100);
}