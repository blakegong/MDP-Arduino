#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "Arduino.h"

class Constants {
public:
	// Digital Pins
	// Digital 2   M1INA     	 Motor 1 *direction input A
	static const unsigned char INA1 = 2;
	// Digital 3   M1_ENCODER_A  Motor 1 *encoder input A
	static const unsigned char M1_ENCODER_A = 3;
	// Digital 4   M1INB	     Motor 1 *direction input B
	static const unsigned char INB1 = 4;
	// Digital 5   M2_ENCODER_A  Motor 2 *encoder input A
	static const unsigned char M2_ENCODER_A = 5;
	static const unsigned char UL2_PWM = 6;
	// Digital 7   M2INA    	 Motor 2 *direction input A
	static const unsigned char INA2 = 7;
	// Digital 8   M2INB    	 Motor 2 *direction input B
	static const unsigned char INB2 = 8;
	// Digital 9   M1PWM	     Motor 1 *speed input
	// Digital 10  M2PWM    	 Motor 2 *speed input
	static const unsigned char UL1_PWM = 11;
	static const unsigned char UL2_TRIG = 12;
	static const unsigned char UL1_TRIG = 13;

	// Analog Pins
	static const unsigned char IR_SHORT_FL = A2;
	static const unsigned char IR_SHORT_FM = A3;
	static const unsigned char IR_SHORT_FR = A4;
	static const unsigned char IR_SHORT_L = A5;

	// static const unsigned char IR_LONG_F = A4;
	static const unsigned char IR_LONG_L = A4;
};

#endif