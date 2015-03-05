#include "Pin.h"
#include "MotionController.h"
#include "SensorController.h"

MotionController* mc;
SensorController* sc;

int cycles = 0;

void setup()
{
    Serial.begin(9600);

    mc = new MotionController();
    sc = new SensorController();
}

void loop() {
    cycles++;
    mc->moveForward(576);
    mc->turnLeft();
    mc->moveForward(576);
    mc->turnRight();
    mc->moveForward(576);
    mc->turnRight();
    mc->moveForward(576);
    mc->turnRight();
    mc->moveForward(576);
    mc->turnRight();
    mc->moveForward(576);
    mc->turnLeft();
    mc->moveForward(576);
    mc->turnLeft();
    mc->moveForward(576);
    mc->turnLeft();
    // mc->moveForward(580);
    // mc->spinMotor(150);
    // mc->readCounts();
    // mc->turnLeft();
    // mc->turnRight();
    if (cycles >= 10)
        while (1);
    delay(1000);
}