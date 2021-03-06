#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H

#include "Arduino.h"
#include <ServoTimer2.h>

class SensorController {
public:
	SensorController();
	int getAnalogReading(unsigned char pin);
	void printSensorFeedback();
	void printSensorRawData();
	unsigned char getIRGrids(unsigned char pin);
	int getUl(unsigned char ulPwm, unsigned char ulTrig);
	float getUlCM(unsigned char ulPwm, unsigned char ulTrig);
	void setServo(unsigned char direction);
	void setServoRead(bool isServoRead);
	ServoTimer2 servo;

private:
	float getIRLongCM(unsigned char pin);
	float getIRShortCM(unsigned char pin);
	
	void initIR();
	void initUltrasonic();
	void initServo();
	unsigned char servoDirection;
	bool isServoRead;
};

#endif