#ifndef FLOWCONTROLLER_H
#define FLOWCONTROLLER_H

#include <MotionController.h>
#include <SensorController.h>

class FlowController {
public:
	FlowController(MotionController* motionController, SensorController* sensorController);
	void executeCommand();
	void executeFastRun();
	void fetchSerial();
	void finish();
	void startFSM();
	void warmUp(); // Getting initial PID reading
	void waitForFastRun();
	void waitForStart();
	void writeSerial();
private:
	int state;
	static const int warmUpState = 0, waitForStartState = 1, fetchSerialState = 2, executeCommandState = 3, writeSerialState = 4, waitForFastRunState = 5, executeFastRunState = 6, finishState = 7;
	MotionController* motionController;
	SensorController* sensorController;

	unsigned char movementType;   //Forward/Backward/Clockwise/AntiClockwise
	unsigned char movementAmount; //could be grids or rotation angle
};

#endif