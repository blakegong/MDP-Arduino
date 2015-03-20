#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "Arduino.h"

class SensorController {
public:
	SensorController();
	int getAnalogReading(unsigned char pin);
	void printSensorFeedback();
	void printSensorFeedbackCalibration();
	unsigned char getIRGrids(unsigned char pin);
	int getUl(unsigned char ulPwm, unsigned char ulTrig);

	float getUlCM(unsigned char ulPwm, unsigned char ulTrig);

private:
	float getIRLongCM(unsigned char pin);
	float getIRShortCM(unsigned char pin);
	
	void initIR();
	void initUltrasonic();
};

#endif