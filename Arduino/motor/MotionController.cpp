#include <MotionController.h>

const long brakeTicks = 60;
volatile long MotionController::M1Count = 0;
volatile long MotionController::M2Count = 0;

MotionController::MotionController() {
	M1Count = M2Count = 0;
	this->motorShield.init();
	initPid();
}

bool MotionController::executeCommand(String command) {
	int count = 0, amount = 0;
	unsigned char movement = 0;
	while (command[count] != 10 && command[count] != 0) {
		if ((command[count] == 'F' || command[count] == 'B'
		        || command[count] == 'C' || command[count] == 'A')) {
			movement = command[count];
		} else {
			amount = amount * 10 + command[count] - 48;
			if ((command[count + 1] == 10 || command[count + 1] == 0
			        || command[count + 1] == 'F' || command[count + 1] == 'B'
			        || command[count + 1] == 'C' || command[count + 1] == 'A')) {
				switch (movement) {
				case 'F':
					this->moveForwardGrids(amount);
					break;
				case 'B':
					this->moveBackwardGrids(amount);
					break;
				case 'C':
					for (int i = 0; i < amount; i++)
						this->turnRight();
					break;
				case 'A':
					for (int i = 0; i < amount; i++)
						this->turnLeft();
					break;
				}
				amount = 0;
			}
		}

		if (Constants::isDebug) {
			Serial.print("count="); Serial.println(count);
			Serial.print("movement="); Serial.println(movement);
			Serial.print("amount="); Serial.println(amount);
		}

		count++;
	}

	return true;
}

void MotionController::initPid() {
	this->pidLeft = new PID(&(this->InputLeft), &(this->OutputLeft), &(this->SetpointLeft), MotionController::kp, MotionController::ki, MotionController::kd, DIRECT);
	this->pidRight = new PID(&(this->InputRight), &(this->OutputRight), &(this->SetpointRight), MotionController::kp, MotionController::ki, MotionController::kd, DIRECT);

	this->SetpointLeft = 0;
	this->SetpointRight = 0;

	this->pidLeft->SetOutputLimits(0, 400 - MotionController::setSpeed);
	this->pidRight->SetOutputLimits(0, 400 - MotionController::setSpeed);

	this->pidLeft->SetMode(AUTOMATIC);
	this->pidRight->SetMode(AUTOMATIC);
}

void MotionController::M1CountInc() {
	MotionController::M1Count++;
}

void MotionController::M2CountInc() {
	MotionController::M2Count++;
}

void MotionController::moveBackward(long ticks) {
	if (M1Count > M2Count) {
		M1Count = M1Count - M2Count;
		M2Count = 0;
	} else {
		M2Count = M2Count - M1Count;
		M1Count = 0;
	}
	long prevCount = M1Count + M2Count;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * (ticks - brakeTicks)) {
		updatePid();
		motorShield.setSpeeds(-MotionController::setSpeed - this->OutputLeft + this->OutputRight, -MotionController::setSpeed + this->OutputLeft - this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}
	long M1prev = 0;
	long M2prev = 0;
	// motorShield.setBrakes(100, 100);
	for (int i = 0; i < 20; i++) {
		motorShield.setBrakes(100, 100);
		delay(5);
		motorShield.setBrakes(300, 300);
		delay(5);
		motorShield.setBrakes(400, 400);
		delay(5);
	}

	if (Constants::isDebug) {
		delay(300);
		Serial.print("Final readings: ");
		printCounts();
		Serial.println();
	}
}

void MotionController::moveBackwardGrids(long grids) {
	this->moveBackward(grids * 576);
}

void MotionController::moveForward(long ticks) {
	if (M1Count > M2Count) {
		M1Count = M1Count - M2Count;
		M2Count = 0;
	} else {
		M2Count = M2Count - M1Count;
		M1Count = 0;
	}
	long prevCount = M1Count + M2Count;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * (ticks - brakeTicks)) {
		updatePid();
		motorShield.setSpeeds(MotionController::setSpeed + this->OutputLeft - this->OutputRight, MotionController::setSpeed - this->OutputLeft + this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}
	long M1prev = 0;
	long M2prev = 0;
	// motorShield.setBrakes(100, 100);
	for (int i = 0; i < 20; i++) {
		motorShield.setBrakes(100, 100);
		delay(5);
		motorShield.setBrakes(300, 300);
		delay(5);
		motorShield.setBrakes(400, 400);
		delay(5);
	}

	if (Constants::isDebug) {
		delay(300);
		Serial.print("Final readings: ");
		printCounts();
		Serial.println();
	}
}

void MotionController::moveForwardGrids(long grids) {
	this->moveForward(grids * 576);
}

void MotionController::turnLeft() {
	if (Constants::isDebug)
		Serial.println("Entering MotionController::turnLeft.");
	// long ticks = 810;
	long ticks = 792;
	if (M1Count > M2Count) {
		M1Count = M1Count - M2Count;
		M2Count = 0;
	} else {
		M2Count = M2Count - M1Count;
		M1Count = 0;
	}
	long prevCount = M1Count + M2Count;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * (ticks - brakeTicks)) {
		updatePid();
		motorShield.setSpeeds(-150 - this->OutputLeft + this->OutputRight, 150 - this->OutputLeft + this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}
	long M1prev = 0;
	long M2prev = 0;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * ticks && MotionController::M1Count != M1prev && MotionController::M2Count != M2prev) {
		updatePid();
		motorShield.setBrakes(-100 + this->OutputLeft - this->OutputRight, 100 + this->OutputLeft - this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
		M1prev = MotionController::M1Count;
		M2prev = MotionController::M2Count;
	}
	if (Constants::isDebug)
		Serial.println("Exiting MotionController::turnLeft.");
}

void MotionController::turnRight() {
	if (Constants::isDebug)
		Serial.println("Entering MotionController::turnRight.");
	// long ticks = 810;
	long ticks = 792;
	if (M1Count > M2Count) {
		M1Count = M1Count - M2Count;
		M2Count = 0;
	} else {
		M2Count = M2Count - M1Count;
		M1Count = 0;
	}
	long prevCount = M1Count + M2Count;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * (ticks - brakeTicks)) {
		updatePid();
		motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, -150 + this->OutputLeft - this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}
	long M1prev = 0;
	long M2prev = 0;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * ticks && MotionController::M1Count != M1prev && MotionController::M2Count != M2prev) {
		updatePid();
		motorShield.setBrakes(100 - this->OutputLeft + this->OutputRight, -100 - this->OutputLeft + this->OutputRight);

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
		M1prev = MotionController::M1Count;
		M2prev = MotionController::M2Count;
	}
	if (Constants::isDebug)
		Serial.println("Exiting MotionController::turnRight.");
}

void MotionController::printInOut() {
	Serial.print("\tInputLeft: ");
	Serial.print(this->InputLeft);
	Serial.print("\tOutputLeft: ");
	Serial.print(this->OutputLeft);
	Serial.print("\tInputRight: ");
	Serial.print(this->InputRight);
	Serial.print("\tOutputRight: ");
	Serial.print(this->OutputRight);
}

void MotionController::printCounts() {
	Serial.print("\tM1: ");
	Serial.print(M1Count);
	Serial.print("\tM2: ");
	Serial.print(M2Count);
}

void MotionController::updatePid() {
	this->InputLeft = MotionController::M1Count - MotionController::M2Count;
	this->InputRight = MotionController::M2Count - MotionController::M1Count;
	this->pidLeft->Compute();
	this->pidRight->Compute();
}