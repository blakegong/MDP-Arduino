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
			motionController->turnLeft();
		break;
	case 65:
		for (int i = 0; i < movementAmount; i++)
			motionController->turnRight();
		break;
	default: ;
	}
	state = FlowController::fetchSerialState;
}

void FlowController::executeFastRun() {

}

void FlowController::fetchSerial() {
	String incomingControl = ""; //storing the control information

	int count = 1;
	while (state == FlowController::fetchSerialState) {
		while (!Serial); //initialize Serial and wait for port to open
		if (Serial.available() > 0) {
			char i = 'x';  //just a dummy value for the sake of initialization
			int exponent = 1;
			count = 1;    //to keep track of index in the incomingControlString
			movementAmount = 0;

			incomingControl = Serial.readString();  //Simulate the incoming Data from Raspberry Pi

			//Serial.println("movement Amount is "+ incomingControl);

			movementType = incomingControl[0];  //type of movement
			//Serial.println(movementType);

			//Detect the null character in the array
			while (i != '\0')
				i = incomingControl[++count];  //start from index 1

			count = count - 1; //remove the count for the control character from Array
			// e.g F234 means move forwards 234 grids.. remove 'F'

			//construct a multiplier e.g: 10^2
			for (int i = 0; i < (count - 1); i++) {
				exponent *= 10;
			}

			for (int i = 1; i <= count; i++) {
				movementAmount += (incomingControl[i] - '0') * exponent;
				exponent /= 10;
			}

			state = FlowController::executeCommandState;

			Serial.flush();


			// Serial.println("---------------------------------------------------------------------");
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

}