#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Arduino.h"

class Constants {
public:
	// Digital Pins
	// Digital 2   M1INA     	 Motor 1 *direction input A
	static const unsigned char INA1 = 2;
	// Digital 3   Timer2 Library
	// Digital 4   M1INB	     Motor 1 *direction input B
	static const unsigned char INB1 = 4;
	// Digital 5   Servo Pin
	static const unsigned char SERVO = 5;
	static const unsigned char MR_ENCODER_A = 19;
	// static const unsigned char UL_RIGHT_PWM = 6; //green
	// Digital 7   M2INA    	 Motor 2 *direction input A
	static const unsigned char INA2 = 7;
	// Digital 8   M2INB    	 Motor 2 *direction input B
	static const unsigned char INB2 = 8;
	// Digital 9   M1PWM	     Motor 1 *speed input
	// Digital 10  M2PWM    	 Motor 2 *speed input
	// Digital 11  Timer2 Library
	// Digital 12  M1_ENCODER_A  Motor 1 *encoder input A
	static const unsigned char ML_ENCODER_A = 13;
	// static const unsigned char UL_LEFT_TRIG = 13;

	// Analog Pins
	static const unsigned char IRS_FL = A2;
	static const unsigned char IRS_FM = A4;
	static const unsigned char IRS_FR = A3;
	static const unsigned char IRS_F = A0;
	static const unsigned char IRS_R = A1;

	// static const unsigned char IR_LONG_F = A4;
	static const unsigned char IRL = A5;

	static const bool isDebug = false;

	static const unsigned char DIRECT_F = 0;
	static const unsigned char DIRECT_R = 1;
	static const unsigned char DIRECT_B = 2;
	static const unsigned char DIRECT_L = 3;
};

#endif