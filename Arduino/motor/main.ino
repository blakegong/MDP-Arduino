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

    PCintPort::attachInterrupt(Constants::ML_ENCODER_A, MotionController::MLCountInc, CHANGE);
    PCintPort::attachInterrupt(Constants::MR_ENCODER_A, MotionController::MRCountInc, CHANGE);

    sensorController = new SensorController();
    motionController = new MotionController(sensorController);

    flowController = new FlowController(motionController, sensorController);
}

void loop() {
    flowController->state = FlowController::waitForStartState;
    // flowController->state = FlowController::testState;
    // flowController->state = FlowController::testForCheckListState;
    flowController->startFSM();
}