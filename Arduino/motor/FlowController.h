#ifndef FLOWCONTROLLER_H
#define FLOWCONTROLLER_H

#include <MotionController.h>
#include <SensorController.h>

class FlowController {
public:
	static const int waitForStartState = 1, fetchSerialState = 2,
	                 executeCommandState = 3, writeSerialState = 4, waitForFastRunState = 5,
	                 executeFastRunState = 6, finishState = 7, testState = 100;
	int state;
	FlowController(MotionController* motionController, SensorController* sensorController);
	void executeCommand();
	void executeFastRun();
	void fetchSerial();
	void finish();
	void startFSM();
	void test();
	void testForCheckList();
	void waitForFastRun();
	void waitForStart();
	void writeSerial();
private:
	MotionController* motionController;
	SensorController* sensorController;

	String command;
};

#endif