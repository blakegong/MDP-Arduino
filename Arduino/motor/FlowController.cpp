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
					for (int i = 0; i < amount; i++)
						motionController->turn(true);
					break;
				case 'A':
					for (int i = 0; i < amount; i++)
						motionController->turn(false);
					break;
				case 'Z':
					motionController->calibratePos(amount);
					break;
				case 'S':
					sensorController->printSensorRawData();
					state = FlowController::writeSerialState;
					break;
				}
				amount = 0;
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
		case FlowController::warmUpState:
			this->warmUp();
			break;
		case FlowController::writeSerialState:
			this->writeSerial();
			break;
		}
	}
}

void FlowController::test() {
	// for(int i = 0; i < 400; i++) {
	// 	motionController->motorShield.setSpeeds(i, i);
	// 	// motionController->motorShield.setM1Speed(i);
	// 	delay(10);
	// }
	motionController->motorShield.setSpeeds(350, 350);
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
	Serial.print("IR Short Front Left Reading: ");
	Serial.print(sensorController->getAnalogReading(Constants::IRS_FL));
	Serial.print("IR Short Front Right Reading: ");
	Serial.println(sensorController->getAnalogReading(Constants::IRS_FR));
	delay(500);
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

			if (command.startsWith("C")) {
				int amount = 0;
				int count = 2;
				while (command[count] != 10 && command[count] != 0) {
					if (command[count] >= 48 && command[count] <= 57)
						amount = amount * 10 + command[count] - 48;
					count++;
				}
				switch (command[1]) {
				case 'F':
					motionController->setForwardTicks(amount);
				case 'B':
					motionController->setBackwardTicks(amount);
				case 'C':
					motionController->setClockwiseTicks(amount);
				case 'A':
					motionController->setAntiClockwiseTicks(amount);
				}
			}
		}
	}
}

void FlowController::warmUp() {
	while (state == FlowController::warmUpState) {
		motionController->calibratePos(1);
		motionController->turn(false);
		motionController->calibratePos(1);
		motionController->turn(true);
		motionController->calibratePos(1);
		delay(500);
		int clockwiseTicks = 730;
		int antiClockwiseTicks = 730;
		unsigned char offset = 64;
		int caliResult = 0;
		for (int i = 0; i < 6; i++) {
			motionController->setClockwiseTicks(clockwiseTicks);
			motionController->setAntiClockwiseTicks(antiClockwiseTicks);

			motionController->turn(false);
			delay(500);
			caliResult = motionController->calibratePos(1);
			delay(200);
			if (caliResult > 0)
				antiClockwiseTicks += offset;
			else
				antiClockwiseTicks -= offset;

			motionController->turn(true);
			delay(500);
			caliResult = motionController->calibratePos(1);
			delay(200);
			if (caliResult < 0)
				clockwiseTicks += offset;
			else
				clockwiseTicks -= offset;

			offset /= 2;
		}

		Serial.print("Final clockwiseTicks: ");
		Serial.print(clockwiseTicks);
		Serial.print(" Final antiClockwiseTicks: ");
		Serial.print(antiClockwiseTicks);
		Serial.println();

		state = FlowController::waitForStartState;
	}
}

void FlowController::writeSerial() {
	while (state == FlowController::writeSerialState) {
		sensorController->printSensorFeedback();
		Serial.println();
		Serial.flush();
		state = FlowController::fetchSerialState;
	}
}