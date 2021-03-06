#include <FlowController.h>
#include <Constants.h>

FlowController::FlowController(MotionController* motionController, SensorController* sensorController) {
	this->motionController = motionController;
	this->sensorController = sensorController;
}

void FlowController::executeCommand() {
	while (state == FlowController::executeCommandState) {
		int count = 0, amount = 0;
		unsigned char movement = 0;
		if (command[0] == 'O') {
			sensorController->setServoRead(false);
			count++;
		} else {
			sensorController->setServoRead(true);
		}
		while (command[count] != 10 && command[count] != 0) {
			if (command[count] >= 48 && command[count] <= 57)
				amount = amount * 10 + command[count] - 48;
			else
				movement = command[count];

			if ((command[count + 1] >= 'A' && command[count + 1] <= 'Z') || (command[count + 1] == 10 || command[count + 1] == 0)) {
				switch (movement) {
				case 'F':
					motionController->moveForwardGrids(amount);
					break;
				case 'B':
					motionController->moveBackwardGrids(amount);
					break;
				case 'C':
					for (int i = 0; i < amount; i++) {
						if (i > 0)
							delay(300);
						motionController->turn(true);
					}
					break;
				case 'A':
					for (int i = 0; i < amount; i++) {
						if (i > 0)
							delay(300);
						motionController->turn(false);
					}
					break;
				case 'Z':
					motionController->calibratePos(amount, true);
					break;
				case 'S':
					sensorController->printSensorRawData();
					break;
				case 'U':
					motionController->calibrateTurn();
					break;
				case 'X':
					motionController->calibrateMove(amount);
					break;
				case 'Y':
					motionController->calibratePos(amount, true, false);
					break;
				case 'D':
					delay(100);
					break;
				}
				amount = 0;
				// delay(50);
			}
			count++;
		}

		state = FlowController::writeSerialState;
	}
}

void FlowController::executeFastRun() {

}

void FlowController::fetchSerial() {
	while (state == FlowController::fetchSerialState) {
		while (!Serial);
		if (Serial.available() > 0) {
			delay(10);
			command = Serial.readString();
			Serial.flush();

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
		case FlowController::testState:
			this->test();
			break;
		case FlowController::waitForFastRunState:
			this->waitForFastRun();
			break;
		case FlowController::waitForStartState:
			this->waitForStart();
			break;
		case FlowController::writeSerialState:
			this->writeSerial();
			break;
		}
	}
}

void FlowController::test() {
	for (int i = 0; i < 3; i++) {
		// sensorController->servo.write(9999);
		// delay(3000);
		// sensorController->servo.write(0);
		// for(int i = 0; i < 400; i++) {
		// 	motionController->motorShield.setSpeeds(i, i);
		// 	// motionController->motorShield.setM1Speed(i);
		// 	delay(10);
		// }
		// motionController->motorShield.setSpeeds(350, 350);
		// motionController->moveForwardGrids(1);
		// sensorController->printSensorFeedbackCalibration();

		// motionController->moveTicks(3000, true);

		// Serial.print("UL Left Reading: ");
		// Serial.println(sensorController->getUlCM(Constants::UL_LEFT_PWM, Constants::UL_LEFT_TRIG));
		// delay(100);
		// Serial.print("UL Right Reading: ");
		// Serial.println(sensorController->getUlCM(Constants::UL_RIGHT_PWM, Constants::UL_RIGHT_TRIG));

		// Serial.print("pIR Short Front Middle Reading: ");
		// Serial.print(sensorController->getAnalogReading(Constants::IR_SHORT_FM));
		// Serial.print("IR Short Front Left Reading: ");
		// Serial.print(sensorController->getAnalogReading(Constants::IRS_FL));
		// Serial.print("IR Short Front Right Reading: ");
		// Serial.println(sensorController->getAnalogReading(Constants::IRS_FR));
		// delay(3000);
		// for (int i = 0; i < 100; i++) {
		// 	motionController->motorShield.setSpeeds(100, -100);
		// 	delay(5);
		// 	motionController->motorShield.setBrakes(400, 400);
		// 	delay(5);
		// }
		// for (int i = 0; i < 100; i++) {
		// 	motionController->motorShield.setSpeeds(200, -200);
		// 	delay(5);
		// 	motionController->motorShield.setBrakes(400, 400);
		// 	delay(5);
		// }
		for (int i = 0; i < 100; i++) {
			motionController->motorShield.setSpeeds(-30, 30);
			delay(5);
			motionController->motorShield.setBrakes(400, 400);
			delay(5);
		}
	}
	while (true) {}
}

void FlowController::waitForFastRun() {

}

void FlowController::waitForStart() {
	while (state == FlowController::waitForStartState) {
		while (!Serial);
		if (Serial.available() > 0) {
			command = Serial.readString();
			Serial.flush();

			if (command.startsWith("S")) {
				motionController->resetCounts();
				state = FlowController::writeSerialState;
			}
		}
	}
}

void FlowController::writeSerial() {
	while (state == FlowController::writeSerialState) {
		sensorController->printSensorFeedback();
		Serial.print(" X: ");
		Serial.print(motionController->getPosX());
		Serial.print(" Y: ");
		Serial.print(motionController->getPosY());
		Serial.print(" D: ");
		switch (motionController->getDirection()) {
		case Constants::DIRECT_F:
			Serial.print("F");
			break;
		case Constants::DIRECT_R:
			Serial.print("R");
			break;
		case Constants::DIRECT_B:
			Serial.print("B");
			break;
		case Constants::DIRECT_L:
			Serial.print("L");
			break;
		}


		Serial.println();

		sensorController->printSensorRawData();

		Serial.flush();
		state = FlowController::fetchSerialState;
	}
}