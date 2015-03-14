#include <FlowController.h>

FlowController::FlowController(MotionController* motionController, SensorController* sensorController) {
	this->motionController = motionController;
	this->sensorController = sensorController;
}

void FlowController::executeCommand() {
	while (state == FlowController::executeCommandState) {
		if (motionController->executeCommand(command))
			state = FlowController::writeSerialState;
	}
}

void FlowController::executeFastRun() {

}

void FlowController::fetchSerial() {
	while (state == FlowController::fetchSerialState) {
		while (!Serial); //initialize Serial and wait for port to open
		if (Serial.available() > 0) {
			command = Serial.readString();  //Simulate the incoming Data from Raspberry Pi
			Serial.flush();

			// for (int i = 0; i < command.length(); i++)
			// 	Serial.println((int)command[i]);

			state = FlowController::executeCommandState;
		}
	}
}

void FlowController::finish() {

}

void FlowController::startFSM() {
	while (1) {
		switch (state) {
		case FlowController::executeCommandState:
			this->executeCommand();
			break;
		case FlowController::executeFastRunState:
			this->executeFastRun();
			break;
		case FlowController::fetchSerialState:
			this->fetchSerial();
			break;
		case FlowController::finishState:
			this->finish();
			break;
		case FlowController::waitForFastRunState:
			this->waitForFastRun();
			break;
		case FlowController::waitForStartState:
			this->waitForStart();
			break;
		case FlowController::warmUpState:
			this->warmUp();
			break;
		case FlowController::writeSerialState:
			this->writeSerial();
			break;
		}
	}
}

void FlowController::waitForFastRun() {

}

void FlowController::waitForStart() {

}

void FlowController::warmUp() {

}

void FlowController::writeSerial() {
	// Serial.println("pHello RaspberryPi, hello PC! ");
	while (state == FlowController::writeSerialState) {
		// Serial.write("pHello RPi!\n");
		sensorController->printSensorFeedback();
		// sensorController->printSensorFeedbackCalibration();
		state = FlowController::fetchSerialState;
	}
}