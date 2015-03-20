#include <MotionController.h>

const long brakeTicks = 60;

volatile long MotionController::MLCount;
volatile long MotionController::MRCount;

MotionController::MotionController(SensorController* sensorController) {
	this->sensorController = sensorController;
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	this->motorShield.init();
	this->initPid();
	this->direction = 0;
	this->posX = 0;
	this->posY = 0;
}

void MotionController::calibratePos(int grids) {
	int l, r, lTarget, rTarget;
	switch (grids) {
	case 1:
		lTarget = 625;
		rTarget = 620;
		break;
	case 2:
		lTarget = 295;
		rTarget = 292;
		break;
	case 3:
		lTarget = 190;
		rTarget = 185;
		break;
	default:
		return;
	}
	do {
		l = this->sensorController->getAnalogReading(Constants::IR_SHORT_FL) - lTarget;
		r = this->sensorController->getAnalogReading(Constants::IR_SHORT_FR) - rTarget;
		motorShield.setSpeeds(- 3 * l, - 3 * r);
		delay(5);
		motorShield.setBrakes(300, 300);
		delay(10);
	} while ((abs(l) > 3) || (abs(r) > 3));
}

int MotionController::getPosX() {
	return this->posX;
}

int MotionController::getPosY() {
	return this->posY;
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
	this->moveTicks(grids * 586, false);
	switch (this->direction) {
	case Constants::DIRECT_F:
		this->posY -= grids;
		break;
	case Constants::DIRECT_R:
		this->posX -= grids;
		break;
	case Constants::DIRECT_B:
		this->posY += grids;
		break;
	case Constants::DIRECT_L:
		this->posX += grids;
		break;
	}
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
		delay(100);
		motorShield.setSpeeds(150, 150);
		delay(50);
	} else {
		motorShield.setSpeeds(-80, -80);
		delay(100);
		motorShield.setSpeeds(-150, -150);
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
	this->moveTicks(grids * 586, true);
	switch (this->direction) {
	case Constants::DIRECT_F:
		this->posY += grids;
		break;
	case Constants::DIRECT_R:
		this->posX += grids;
		break;
	case Constants::DIRECT_B:
		this->posY -= grids;
		break;
	case Constants::DIRECT_L:
		this->posX -= grids;
		break;
	}
}

void MotionController::setPosition(int posX, int posY) {
	this->posX = posX;
	this->posY = posY;
}

void MotionController::turn(bool isClockwise) {
	if (Constants::isDebug)
		Serial.println("Entering MotionController::turn.");

	long ticks = isClockwise ? 830 : 828;

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

	switch (direction) {
	case Constants::DIRECT_F:
		direction = isClockwise ? Constants::DIRECT_R : Constants::DIRECT_L;
		break;
	case Constants::DIRECT_R:
		direction = isClockwise ? Constants::DIRECT_B : Constants::DIRECT_F;
		break;
	case Constants::DIRECT_B:
		direction = isClockwise ? Constants::DIRECT_L : Constants::DIRECT_R;
		break;
	case Constants::DIRECT_L:
		direction = isClockwise ? Constants::DIRECT_F : Constants::DIRECT_B;
		break;
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