#include <MotionController.h>

volatile long MotionController::MLCount;
volatile long MotionController::MRCount;

MotionController::MotionController(SensorController* sensorController) {
	this->sensorController = sensorController;
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	this->motorShield.init();
	this->initPid();
	this->initTicks();
	this->direction = Constants::DIRECT_L;
	this->posX = 1;
	this->posY = 1;
	this->lTarget[1] = 569;
	this->lTarget[2] = 282;
	this->mTarget[1] = 601;
	this->mTarget[2] = 289;
	this->rTarget[1] = 589;
	this->rTarget[2] = 286;
}

int MotionController::calibratePos(int amount, bool isTurn, bool isTimeoutEnable) {
	int lGrids = amount / 100;
	amount %= 100;
	int mGrids = amount / 10;
	amount %= 10;
	int rGrids = amount;
	int l, r, value;
	bool isFirstTime = true;
	int count  = 0;
	bool isCalibrating = true;
	while (isCalibrating) {
		if (lGrids && rGrids) {
			l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[lGrids];
			r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[rGrids];
		} else if (lGrids && mGrids) {
			l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[lGrids];
			r = this->sensorController->getAnalogReading(Constants::IRS_FM) - mTarget[mGrids];
		} else if (mGrids && rGrids) {
			l = this->sensorController->getAnalogReading(Constants::IRS_FM) - mTarget[mGrids];
			r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[rGrids];
		}
		if (isFirstTime) {
			if (isTurn)
				value = l - r;
			else
				value = l + r;
			isFirstTime = false;
		}

		if (isTimeoutEnable) {
			if (l > 32)
				l = 32;
			if (l < -32)
				l = -32;
			if (r > 32)
				r = 32;
			if (r < -32)
				r = -32;
		}

		if (l < 8 && l > 0)
			l = 8;
		if (l > -8 && l < 0)
			l = -8;
		if (r < 8 && r > 0)
			r = 8;
		if (r > -8 && r < 0)
			r = -8;

		motorShield.setSpeeds(- 5 * l, - 5 * r);
		delay(5);
		motorShield.setBrakes(400, 400);
		delay(5);
		count++;
		if ((abs(l) < 3) && (abs(r) < 3)) {
			delay(5);
			if (lGrids && rGrids) {
				l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[lGrids];
				r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[rGrids];
			} else if (lGrids && mGrids) {
				l = this->sensorController->getAnalogReading(Constants::IRS_FL) - lTarget[lGrids];
				r = this->sensorController->getAnalogReading(Constants::IRS_FM) - mTarget[mGrids];
			} else if (mGrids && rGrids) {
				l = this->sensorController->getAnalogReading(Constants::IRS_FM) - mTarget[mGrids];
				r = this->sensorController->getAnalogReading(Constants::IRS_FR) - rTarget[rGrids];
			}
			delay(40);
			if ((abs(l) < 5) && (abs(r) < 5)) {
				isCalibrating = false;
			}
		}
		if (count > 100 && isTimeoutEnable)
			isCalibrating = false;
	}
	MotionController::MLCount = 0;
	MotionController::MRCount = 0;
	return value;
}

void  MotionController::calibrateTurn() {
	this->calibratePos(101, true);
	this->turn(false);
	this->calibratePos(101, true);
	// this->turn(true);
	// this->calibratePos(101, true);
	delay(100);
	unsigned char offset = 8;
	int caliResult = 0;
	for (int i = 0; i < 4; i++) {
		this->turn(true);
		delay(100);
		caliResult = this->calibratePos(101, true);
		delay(100);
		if (caliResult < 0)
			clockwiseTicks += offset;
		else
			clockwiseTicks -= offset;

		this->turn(false);
		delay(100);
		caliResult = this->calibratePos(101, true);
		delay(100);
		if (caliResult > 0)
			antiClockwiseTicks += offset;
		else
			antiClockwiseTicks -= offset;

		offset = offset > 1 ? offset /= 2 : 1;
	}

	// Serial.print("Final clockwiseTicks: ");
	// Serial.print(clockwiseTicks);
	// Serial.print(" Final antiClockwiseTicks: ");
	// Serial.print(antiClockwiseTicks);
	// Serial.println();
}

void MotionController::calibrateMove(unsigned char grids) {
	this->calibratePos(202, true);
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
		caliResult = this->calibratePos(202, true);
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

int MotionController::getDirection() {
	return this->direction;
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

	this->pidLeft->SetOutputLimits(0, 50);
	this->pidRight->SetOutputLimits(0, 50);

	this->pidLeft->SetMode(AUTOMATIC);
	this->pidRight->SetMode(AUTOMATIC);
}

void MotionController::initTicks() {
	this->forwardTicks[1] = 493;//515
	this->forwardTicks[2] = 1050;
	this->forwardTicks[5] = 2745;//2800
	this->forwardTicks[7] = 3970;//4060
	int ticksPerGrid = (this->forwardTicks[7] - this->forwardTicks[5]) / 2;
	this->forwardTicks[3] = 2 * this->forwardTicks[5] - this->forwardTicks[7];
	for (int i = 4; i < 20; i++) {
		this->forwardTicks[i] = this->forwardTicks[i - 1] + ticksPerGrid;
	}
	this->clockwiseTicks = 640;
	this->antiClockwiseTicks = 639;
}

void MotionController::MLCountInc() {
	MotionController::MLCount++;
}

void MotionController::MRCountInc() {
	MotionController::MRCount++;
}

bool MotionController::moveBackwardGrids(long grids) {
	if (grids == 0)
		return false;
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
	if (MotionController::MLCount > MotionController::MRCount) {
		MotionController::MLCount = MotionController::MLCount - MotionController::MRCount;
		MotionController::MRCount = 0;
	} else {
		MotionController::MRCount = MotionController::MRCount - MotionController::MLCount;
		MotionController::MLCount = 0;
	}

	long prevCount = MotionController::MLCount + MotionController::MRCount;

	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * ticks) {
		// Take care of this line.
		if (isForward) {
			motorShield.setSpeeds(MotionController::moveSpeed + this->OutputLeft - this->OutputRight, MotionController::moveSpeed - this->OutputLeft + this->OutputRight);
		} else {
			motorShield.setSpeeds(-MotionController::moveSpeed - this->OutputLeft + this->OutputRight, -MotionController::moveSpeed + this->OutputLeft - this->OutputRight);
		}

		updatePid();

		// if (sensorController->getAnalogReading(Constants::IRS_FL) > 220) {
		// 	isForceStop = true;
		// 	break;
		// }
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

	return isForceStop;
}

bool MotionController::fastMoveTicks(long ticks) {
	bool isForceStop = false;
	if (MotionController::MLCount > MotionController::MRCount) {
		MotionController::MLCount = MotionController::MLCount - MotionController::MRCount;
		MotionController::MRCount = 0;
	} else {
		MotionController::MRCount = MotionController::MRCount - MotionController::MLCount;
		MotionController::MLCount = 0;
	}

	long prevCount = MotionController::MLCount + MotionController::MRCount;

	for (int i = 0; i < 30; i++) {
		motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, 150 - this->OutputLeft + this->OutputRight);
		updatePid();
	}

	for (int i = 0; i < 30; i++) {
		motorShield.setSpeeds(400 + this->OutputLeft - this->OutputRight, 400 - this->OutputLeft + this->OutputRight);
		updatePid();
	}

	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * ticks) {
		// Take care of this line.
		motorShield.setSpeeds(MotionController::fastMoveSpeed + this->OutputLeft - this->OutputRight, MotionController::fastMoveSpeed - this->OutputLeft + this->OutputRight);
		updatePid();
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

	if (MotionController::MLCount > MotionController::MRCount) {
		prevCount = MotionController::MLCount - MotionController::MRCount;
		while (MotionController::MLCount - MotionController::MRCount > (prevCount / 2)) {
			motorShield.setSpeeds(0, 50);
			delay(5);
			motorShield.setBrakes(400, 400);
			delay(5);
		}
	} else {
		prevCount = MotionController::MRCount - MotionController::MLCount;
		while (MotionController::MRCount - MotionController::MLCount > (prevCount / 2)) {
			motorShield.setSpeeds(50, 0);
			delay(5);
			motorShield.setBrakes(400, 400);
			delay(5);
		}
	}

	return isForceStop;
}

bool MotionController::moveForwardGrids(long grids) {
	if (grids == 0)
		return false;
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
	if (grids < 2)
		return this->moveTicks(forwardTicks[grids], true);
	else
		return this->fastMoveTicks(forwardTicks[grids]);
}

void MotionController::setPosition(int posX, int posY) {
	this->posX = posX;
	this->posY = posY;
}

void MotionController::turn(bool isClockwise) {
	if (isClockwise) {
		Serial.print(" C: ");
		Serial.print(clockwiseTicks);
	} else {
		Serial.print(" A: ");
		Serial.print(antiClockwiseTicks);
	}
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
	for (int i = 0; i < 30; i++) {
		if (isClockwise) {
			motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, -150 + this->OutputLeft - this->OutputRight);
		} else {
			motorShield.setSpeeds(-150 - this->OutputLeft + this->OutputRight, 150 - this->OutputLeft + this->OutputRight);
		}
		updatePid();
	}
	while (MotionController::MLCount + MotionController::MRCount - prevCount < 2 * ticks) {
		if (isClockwise) {
			motorShield.setSpeeds(MotionController::turnSpeed + this->OutputLeft - this->OutputRight, -MotionController::turnSpeed + this->OutputLeft - this->OutputRight);
		} else {
			motorShield.setSpeeds(-MotionController::turnSpeed - this->OutputLeft + this->OutputRight, MotionController::turnSpeed - this->OutputLeft + this->OutputRight);
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

void MotionController::resetCounts() {
	MotionController::MLCount = MotionController::MRCount = 0;
}

void MotionController::updatePid() {
	this->InputLeft = MotionController::MLCount - MotionController::MRCount;
	this->InputRight = -this->InputLeft;
	this->pidLeft->Compute();
	this->pidRight->Compute();
}
