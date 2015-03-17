#include "Arduino.h"
#include "Constants.h"
#include "PinChangeInt.h"
#include "FlowController.h"
#include "MotionController.h"
#include "SensorController.h"

FlowController* flowController;
MotionController* motionController;
SensorController* sensorController;

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
	flowController->state = FlowController::waitForStartState;
    // flowController->state = FlowController::writeSerialState;
    flowController->startFSM();
}