#ifndef CONSTANTS_H
#define CONSTANTS_H
Class Constants {
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
	static const unsigned char ULTRA_2_PWM = 6;
	// Digital 7   M2INA    	 Motor 2 *direction input A
	static const unsigned char INA2 = 7;
	// Digital 8   M2INB    	 Motor 2 *direction input B
	static const unsigned char INB2 = 8;
	// Digital 9   M1PWM	     Motor 1 *speed input
	// Digital 10  M2PWM    	 Motor 2 *speed input
	static const unsigned char ULTRA_1_PWM = 11;
	static const unsigned char ULTRA_2_TRIG = 12;
	static const unsigned char ULTRA_1_TRIG = 13;

}
#endif