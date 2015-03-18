#include <MotionController.h>

const long brakeTicks = 60;
volatile long MotionController::MLCount = 0;
volatile long MotionController::MRCount = 0;

MotionController::MotionController(SensorController* sensorController) {
	this->sensorController = sensorController;
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	this->motorShield.init();
	this->initPid();
}

void MotionController::calibratePos() {
	int l, r;
	do {
		l = this->sensorController->getAnalogReading(Constants::IR_SHORT_FL) - 625;
		r = this->sensorController->getAnalogReading(Constants::IR_SHORT_FR) - 620;
		motorShield.setSpeeds(- 3 * l, - 3 * r);
		delay(5);
		motorShield.setBrakes(300, 300);
	} while ((abs(l) > 5) || (abs(r) > 5));
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

void MotionController::MLCountInc() {
	MotionController::MLCount++;
}

void MotionController::MRCountInc() {
	MotionController::MRCount++;
}

void MotionController::moveBackwardGrids(long grids) {
	this->moveTicks(grids * 576, false);
}

void MotionController::moveTicks(long ticks, bool isForward) {
	if (MotionController::MLCount > MotionController::MRCount) {
		MotionController::MLCount = MotionController::MLCount - MotionController::MRCount;
		MotionController::MRCount = 0;
	} else {
		MotionController::MRCount = MotionController::MRCount - MotionController::MLCount;
		MotionController::MLCount = 0;
	}

	if (Constants::isDebug) {
		printInOut();
		printCounts();
		Serial.println();
	}

	long prevCount = MotionController::MLCount + MotionController::MRCount;
	if (isForward) {
		motorShield.setSpeeds(80, 80);
		delay(50);
		motorShield.setSpeeds(200, 200);
		delay(50);
	} else {
		motorShield.setSpeeds(-100, -100);
		delay(50);
		motorShield.setSpeeds(-200, -200);
		delay(50);
	}

	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * (ticks - brakeTicks)) {
		// Take care of this line.
		if (isForward) {
			motorShield.setSpeeds(MotionController::setSpeed + this->OutputLeft - this->OutputRight, MotionController::setSpeed - this->OutputLeft + this->OutputRight);
		} else {
			motorShield.setSpeeds(-MotionController::setSpeed - this->OutputLeft + this->OutputRight, -MotionController::setSpeed + this->OutputLeft - this->OutputRight);
		}

		updatePid();

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}

	// Braking
	prevCount = 0;
	while (MotionController::MLCount + MotionController::MRCount - prevCount > 2) {
		prevCount = MotionController::MLCount + MotionController::MRCount;
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
	this->moveTicks(grids * 576, true);
}

void MotionController::turn(bool isClockwise) {
	if (Constants::isDebug)
		Serial.println("Entering MotionController::turn.");

	long ticks = isClockwise ? 805 : 810;

	if (MotionController::MLCount > MotionController::MRCount) {
		MotionController::MLCount = MotionController::MLCount - MotionController::MRCount;
		MotionController::MRCount = 0;
	} else {
		MotionController::MRCount = MotionController::MRCount - MotionController::MLCount;
		MotionController::MLCount = 0;
	}
	long prevCount = MotionController::MLCount + MotionController::MRCount;
	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * (ticks - brakeTicks)) {
		updatePid();
		if (isClockwise) {
			motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, -150 + this->OutputLeft - this->OutputRight);
		} else {
			motorShield.setSpeeds(-150 - this->OutputLeft + this->OutputRight, 150 - this->OutputLeft + this->OutputRight);
		}

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}

	// Braking
	prevCount = 0;
	while (MotionController::MLCount + MotionController::MRCount - prevCount > 2) {
		prevCount = MotionController::MLCount + MotionController::MRCount;
		motorShield.setBrakes(100, 100);
		delay(5);
		motorShield.setBrakes(300, 300);
		delay(5);
		motorShield.setBrakes(400, 400);
		delay(5);
	}

	if (Constants::isDebug)
		Serial.println("Exiting MotionController::turnLeft.");
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
	Serial.print(MotionController::MLCount);
	Serial.print("\tM2: ");
	Serial.print(MotionController::MRCount);
}

void MotionController::resetCounts() {
	MotionController::MLCount = MotionController::MRCount = 0;
}

void MotionController::updatePid() {
	this->InputLeft = MotionController::MLCount - MotionController::MRCount;
	this->InputRight = -this->InputLeft;
	this->pidLeft->Compute();
	this->pidRight->Compute();
}