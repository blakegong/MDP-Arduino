#include <MotionController.h>

const long brakeTicks = 60;
volatile long MotionController::M1Count = 0;
volatile long MotionController::M2Count = 0;

MotionController::MotionController() {
	M1Count = M2Count = 0;
	this->motorShield.init();
	initPid();
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

void MotionController::M1CountInc() {
	MotionController::M1Count++;
}

void MotionController::M2CountInc() {
	M2Count++;
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
		motorShield.setSpeeds(350 + this->OutputLeft - this->OutputRight, 350 - this->OutputLeft + this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
	long M1prev = 0;
	long M2prev = 0;
	motorShield.setBrakes(100, 100);
	// motorShield.setSpeeds(0, 0);
	// while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * ticks && MotionController::M1Count != M1prev && MotionController::M2Count != M2prev) {
	// 	updatePid();
	// 	motorShield.setBrakes(100 - this->OutputLeft + this->OutputRight, 100 + this->OutputLeft - this->OutputRight);

	// 	printInOut();
	// 	printCounts();
	// 	Serial.println();
	// 	M1prev = MotionController::M1Count;
	// 	M2prev = MotionController::M2Count;
	// }

	// delay(300);
	// Serial.print("Final readings: ");
	printCounts();
	Serial.println();
}

void MotionController::moveForwardGrids(long grids) {
	this->moveForward(grids * 576);
}

void MotionController::readCounts() {
	// printCounts();
	// Serial.println();
}

void MotionController::spinMotor(long speed) {
	motorShield.setSpeeds(speed, speed);
}

void MotionController::turnLeft() {
	long ticks = 783;
	// long ticks = 792;
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

		printInOut();
		printCounts();
		// Serial.println();
	}
	long M1prev = 0;
	long M2prev = 0;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * ticks && MotionController::M1Count != M1prev && MotionController::M2Count != M2prev) {
		updatePid();
		motorShield.setBrakes(-100 + this->OutputLeft - this->OutputRight, 100 + this->OutputLeft - this->OutputRight);

		printInOut();
		printCounts();
		// Serial.println();
		M1prev = MotionController::M1Count;
		M2prev = MotionController::M2Count;
	}
}

void MotionController::turnRight() {
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

		printInOut();
		printCounts();
		// Serial.println();
	}
	long M1prev = 0;
	long M2prev = 0;
	while (MotionController::M1Count + MotionController::M2Count - prevCount < 2 * ticks && MotionController::M1Count != M1prev && MotionController::M2Count != M2prev) {
		updatePid();
		motorShield.setBrakes(100 - this->OutputLeft + this->OutputRight, -100 - this->OutputLeft + this->OutputRight);

		printInOut();
		printCounts();
		// Serial.println();
		M1prev = MotionController::M1Count;
		M2prev = MotionController::M2Count;
	}
}

void MotionController::printInOut() {
	// Serial.print("\tInputLeft: ");
	// Serial.print(this->InputLeft);
	// Serial.print("\tOutputLeft: ");
	// Serial.print(this->OutputLeft);
	// Serial.print("\tInputRight: ");
	// Serial.print(this->InputRight);
	// Serial.print("\tOutputRight: ");
	// Serial.print(this->OutputRight);
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