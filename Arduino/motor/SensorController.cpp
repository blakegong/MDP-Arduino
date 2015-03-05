#include <SensorController.h>
#include "Constants.h"

SensorController::SensorController() {
    initIR();
    initUltrasonic();
}

void SensorController::initIR() {
    pinMode(Constants::IR_SHORT_FL, INPUT);
    pinMode(Constants::IR_SHORT_FM, INPUT);
}

void SensorController::initUltrasonic() {
    pinMode(Constants::UL1_TRIG, OUTPUT);
    digitalWrite(Constants::UL1_TRIG, HIGH);
    pinMode(Constants::UL2_TRIG, OUTPUT);
    digitalWrite(Constants::UL2_TRIG, HIGH);

    pinMode(Constants::UL1_PWM, INPUT);
    pinMode(Constants::UL2_PWM, INPUT);
}

unsigned char SensorController::getSensorFeedbackHighByte() {
    unsigned char c = 0;
    c = this->getUl(Constants::UL1_PWM, Constants::UL1_TRIG) ? 1 : 0;
    c = (c << 1) + this->getUl(Constants::UL2_PWM, Constants::UL2_TRIG) ? 1 : 0;
    c = (c << 3) + this->getIRGrids(Constants::IR_LONG_F);
    c = (c << 3) + this->getIRGrids(Constants::IR_LONG_L);
    return c;
}

unsigned char SensorController::getSensorFeedbackLowByte() {
    unsigned char c = 0;
    unsigned char irFL = this->getIRGrids(Constants::IR_SHORT_FL);
    // unsigned char irFM = this->getIRGrids(Constants::IR_SHORT_FM);
    unsigned char irFR = this->getIRGrids(Constants::IR_SHORT_FR);
    unsigned char irL = this->getIRGrids(Constants::IR_SHORT_L);
    return c;
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    // TODO: ditch readings larger than x grids
    float offset = 0;
    switch (pin) {
    case Constants::IR_SHORT_FL:
        offset = 0;
        return (unsigned char) ((getIRShortCM(pin) - offset + 5) / 10); // 5 for rounding
    case Constants::IR_SHORT_FM:
        offset = 0;
        return (unsigned char) ((getIRShortCM(pin) - offset + 5) / 10); // 5 for rounding
    }
}

float SensorController::getIRLongCM(unsigned char pin) {
    return 10650.08 * pow(this->getAnalogReading(pin), -0.935) - 10;
}

float SensorController::getIRShortCM(unsigned char pin) {
    return 12343.85 * pow(this->getAnalogReading(pin), -1.15);
}

int SensorController::getAnalogReading(unsigned char pin) {
    int sum;
    analogRead(pin);
    analogRead(pin);
    sum = analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    return (sum >> 2);
}

bool getUl(unsigned char ulPwm, unsigned char ulTrig) {

}

float SensorController::getUlCM(unsigned char ulPwm, unsigned char ulTrig) {
    uint8_t EnPwmCmd[4] = {0x44, 0x02, 0xbb, 0x01}; 
    digitalWrite(ulTrig, LOW);
    digitalWrite(ulTrig, HIGH);       

    unsigned long distance = pulseIn(ulPwm, LOW);

    if (distance == 50000) {          // the reading is invalid.
        // Serial.print("Invalid");
    } else {
        return (float)distance / 50.0;       // every 50us low level stands for 1cm
    }
}