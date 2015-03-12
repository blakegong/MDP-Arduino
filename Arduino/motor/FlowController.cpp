#include <FlowController.h>

FlowController::FlowController(MotionController* motionController, SensorController* sensorController) {
	this->motionController = motionController;
	this->sensorController = sensorController;
	state = FlowController::fetchSerialState;


	movementType = 0;   //Forward/Backward/Clockwise/AntiClockwise
	movementAmount = 0; //could be grids or rotation angle
}

void FlowController::executeCommand() {
	// Serial.print("executeCommand state");
	// Serial.print(movementType);

	switch (movementType) {
	case 70: motionController->moveForwardGrids(movementAmount);
		break;
	case 66: motionController->moveForwardGrids(movementAmount * 2);
		break;
	case 67:
		for (int i = 0; i < movementAmount; i++)
			motionController->turnRight();
		break;
	case 65:
		for (int i = 0; i < movementAmount; i++)
			motionController->turnLeft();
		break;
	default: ;
	}
	state = FlowController::writeSerialState;
}

void FlowController::executeFastRun() {

}

void FlowController::fetchSerial() {
	String incomingControl = ""; //storing the control information

	while (state == FlowController::fetchSerialState) {
		while (!Serial); //initialize Serial and wait for port to open
		if (Serial.available() > 0) {
			unsigned char c = 'x';  //just a dummy value for the sake of initialization
			int count = 1;    //to keep track of index in the incomingControlString
			movementAmount = 0;

			incomingControl = Serial.readString();  //Simulate the incoming Data from Raspberry Pi
			Serial.flush();

			movementType = incomingControl[0];  //type of movement

			//Detect the null character in the array
			c = incomingControl[count];
			while (c != 10 && c != 0) {
				movementAmount = movementAmount * 10 + c - 48;
				c = incomingControl[++count];
			}

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
		sensorController->printSensorFeedback();
		state = FlowController::fetchSerialState;
	}
}