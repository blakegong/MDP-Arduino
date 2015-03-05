#ifndef FLOWCONTROLLER_H
#define FLOWCONTROLLER_H

#include <MotionController.h>
#include <SensorController.h>

class FlowController {
public:
	FlowController(MotionController motionController, SensorController sensorController);
	void executeCommand();
	void fetchSerial();
	void startFSM();
	void warmUp(); // Getting initial PID reading
	void writeSerial();
private:
	int state;
	static const int warmUpState = 0, waitingForStartState = 1, exploreState = 2, waitingForFastRunState = 3, fastRunState = 4, finishState = 5;
	MotionController motionController;
	SensorController sensorController;
};

#endif