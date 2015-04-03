#include <MotionController.h>

volatile long MotionController::MLCount;
volatile long MotionController::MRCount;

MotionController::MotionController(SensorController* sensorController) {
	this->sensorController = sensorController;
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	this->motorShield.init();
	this->initPid();
	this->direction = Constants::DIRECT_L;
	this->posX = 0;
	this->posY = 0;
	for (int i = 0; i < 20; i++)
		this->forwardTicks[i] = 545 * i;
	this->clockwiseTicks = 680;
	this->antiClockwiseTicks = 680;
	this->lTarget[1] = 595;
	this->lTarget[2] = 282;
	this->lTarget[3] = 200;
	this->mTarget[1] = 604;
	this->mTarget[2] = 286;
	this->rTarget[1] = 604;
	this->rTarget[2] = 286;
	this->rTarget[3] = 199;
}

int MotionController::calibratePos(int grids, bool isTurn) {
	int l, r, value;
	bool isFirstTime = true;
	int count  = 0;
	bool isCalibrating = true;
	while (isCalibrating) {
		l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[grids];
		r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[grids];
		if (isFirstTime) {
			if (isTurn)
				value = l - r;
			else
				value = l + r;
			isFirstTime = false;
		}

		motorShield.setSpeeds(- 5 * l, - 5 * r);
		delay(5);
		motorShield.setBrakes(400, 400);
		delay(5);
		count++;
		if ((abs(l) < 3) && (abs(r) < 3)) {
			delay(5);
			l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[grids];
			r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[grids];
			delay(40);
			if ((abs(l) < 5) && (abs(r) < 5)) {
				isCalibrating = false;
			}
		}
		if (count > 60)
			isCalibrating = false;
	}
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	return value;
}

void  MotionController::calibrateTurn() {
	this->calibratePos(1, true);
	this->turn(false);
	this->calibratePos(1, true);
	this->turn(true);
	this->calibratePos(1, true);
	delay(100);
	unsigned char offset = 16;
	int caliResult = 0;
	for (int i = 0; i < 7; i++) {
		this->clockwiseTicks = clockwiseTicks;
		this->antiClockwiseTicks = antiClockwiseTicks;

		this->turn(false);
		delay(100);
		caliResult = this->calibratePos(1, true);
		delay(100);
		if (caliResult > 0)
			antiClockwiseTicks += offset;
		else
			antiClockwiseTicks -= offset;

		this->turn(true);
		delay(100);
		caliResult = this->calibratePos(1, true);
		delay(100);
		if (caliResult < 0)
			clockwiseTicks += offset;
		else
			clockwiseTicks -= offset;

		offset = offset > 1 ? offset /= 2 : 1;
	}

	// Serial.print("Final clockwiseTicks: ");
	// Serial.print(clockwiseTicks);
	// Serial.print(" Final antiClockwiseTicks: ");
	// Serial.print(antiClockwiseTicks);
	// Serial.println();
}

void MotionController::calibrateMove(unsigned char grids) {
	this->calibratePos(2, true);
	this->turn(false);
	this->turn(false);
	delay(500);
	unsigned char offset = 64;
	int caliResult = 0;
	for (int i = 0; i < 6; i++) {
		bool isForceStop;
		for (int moves = 0; moves < 6; moves += grids) {
			isForceStop = this->moveForwardGrids(grids);
		}
		delay(500);
		caliResult = this->calibratePos(2, true);
		delay(200);

		if (caliResult < 0 && !isForceStop)
			this->forwardTicks[grids] += offset;
		else
			this->forwardTicks[grids] -= offset;

		this->turn(false);
		this->turn(false);
		delay(200);
		offset /= 2;
	}

	// Serial.print("Final ticks for ");
	// Serial.print(grids);
	// Serial.print(" grids: ");
	// Serial.print(forwardTicks[grids]);
	// Serial.println();
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

bool MotionController::moveBackwardGrids(long grids) {
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
	return this->moveTicks(grids * forwardTicks[grids], false);
}

bool MotionController::moveTicks(long ticks, bool isForward) {
	bool isForceStop = false;
	if (ticks == 0)
		return false;
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

	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * ticks) {
		// Take care of this line.
		if (isForward) {
			motorShield.setSpeeds(MotionController::setSpeed + this->OutputLeft - this->OutputRight, MotionController::setSpeed - this->OutputLeft + this->OutputRight);
		} else {
			motorShield.setSpeeds(-MotionController::setSpeed - this->OutputLeft + this->OutputRight, -MotionController::setSpeed + this->OutputLeft - this->OutputRight);
		}

		updatePid();

		// if (sensorController->getAnalogReading(Constants::IRS_FL) > 220) {
		// 	isForceStop = true;
		// 	break;
		// }

		if (Constants::isDebug) {
			printInOut();
			printCounts();
			Serial.println();
		}
	}

	// Braking
	prevCount = 0;
	bool isBrake = true;
	motorShield.setBrakes(400, 400);
	while (isBrake) {
		prevCount = MotionController::MLCount + MotionController::MRCount;
		motorShield.setBrakes(400, 400);
		delay(5);
		if (MotionController::MLCount + MotionController::MRCount - prevCount == 0) {
			delay(50);
			if (MotionController::MLCount + MotionController::MRCount - prevCount == 0) {
				isBrake = false;
			}
		}
	}

	if (Constants::isDebug) {
		delay(300);
		Serial.print("Final readings: ");
		printCounts();
		Serial.println();
	}
	return isForceStop;
}

bool MotionController::moveForwardGrids(long grids) {
	// this->moveTicks(grids * 565, true);
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
	return this->moveTicks(forwardTicks[grids], true);
}

void MotionController::setPosition(int posX, int posY) {
	this->posX = posX;
	this->posY = posY;
}

void MotionController::turn(bool isClockwise) {
	Serial.print(" Current clockwise ticks: ");
	Serial.print(clockwiseTicks);
	Serial.print(" antiClockwise ticks: ");
	Serial.print(antiClockwiseTicks);
	Serial.println();
	long ticks = isClockwise ? clockwiseTicks : antiClockwiseTicks;

	if (MotionController::MLCount > MotionController::MRCount) {
		MotionController::MLCount = MotionController::MLCount - MotionController::MRCount;
		MotionController::MRCount = 0;
	} else {
		MotionController::MRCount = MotionController::MRCount - MotionController::MLCount;
		MotionController::MLCount = 0;
	}
	long prevCount = MotionController::MLCount + MotionController::MRCount;
	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * ticks) {
		int turnSpeed = 250;
		if (isClockwise) {
			motorShield.setSpeeds(turnSpeed + this->OutputLeft - this->OutputRight, -turnSpeed + this->OutputLeft - this->OutputRight);
		} else {
			motorShield.setSpeeds(-turnSpeed - this->OutputLeft + this->OutputRight, turnSpeed - this->OutputLeft + this->OutputRight);
		}
		updatePid();
	}

	// Braking
	prevCount = 0;
	bool isBrake = true;
	motorShield.setBrakes(400, 400);
	while (isBrake) {
		prevCount = MotionController::MLCount + MotionController::MRCount;
		delay(5);
		motorShield.setBrakes(400, 400);
		if (MotionController::MLCount + MotionController::MRCount - prevCount == 0) {
			delay(50);
			if (MotionController::MLCount + MotionController::MRCount - prevCount == 0) {
				isBrake = false;
			}
		}
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

	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
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
