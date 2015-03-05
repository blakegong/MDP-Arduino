#include <SensorController.h>

SensorController::SensorController() {

}

void initUltra

float getIRShortCM(unsigned char pin) {
	return 12343.85 * pow(analogRead(pin), -1.15);
}

float getIRLongCM(unsigned char pin) {
	return 10650.08 * pow(analogRead(pin), -0.935) - 10;
}

float getUltrasonicCM() {

}