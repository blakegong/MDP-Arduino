#ifndef SENSORCONTROLLER_H
#define SENSORCONTROLLER_H
class SensorController {
	public:
		SensorController();
		float getIRShortCM(unsigned char pin);
		float getIRLongCM(unsigned char pin);
		float getUltrasonicCM(unsigned char pin);
};

#endif