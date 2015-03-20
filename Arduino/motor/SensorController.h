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

	float getIRLongCM(unsigned char pin);
	float getIRShortCM(unsigned char pin);

private:
	
	float getUlCM(unsigned char ulPwm, unsigned char ulTrig);
	void initIR();
	void initUltrasonic();
};

#endif