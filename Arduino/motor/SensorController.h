#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "Arduino.h"

class SensorController {
public:
	SensorController();
	void printSensorFeedback();
	void printSensorFeedbackCalibration();
	unsigned char getIRGrids(unsigned char pin);
	bool getUl(unsigned char ulPwm, unsigned char ulTrig);

private:
	int getAnalogReading(unsigned char pin);
	float getIRLongCM(unsigned char pin);
	float getIRShortCM(unsigned char pin);
	float getUlCM(unsigned char ulPwm, unsigned char ulTrig);
	void initIR();
	void initUltrasonic();
};

#endif