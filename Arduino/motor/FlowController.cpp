#include <FlowController.h>

FlowController::FlowController(MotionController motionController, SensorController sensorController) {
	this->motionController = motionController;
	this->sensorController = sensorController;
}

void FlowController::executeCommand() {

}

void FlowController::fetchSerial() {

}

void FlowController::startFSM() {
	while (state == FlowController::waitingForStartState) {
		this->fetchSerial();
	}
	while (state == FlowController::exploreState) {
		this->executeCommand();
		this->writeSerial();
		this->fetchSerial();
	}
	while (state == FlowController::waitingForFastRunState) {
		this->fetchSerial();
	}
	while (state == FlowController::fastRunState) {
		this->executeCommand();
		this->fetchSerial();
	}
	while (state == FlowController::finishState) {

	}
}

void FlowController::writeSerial() {

}