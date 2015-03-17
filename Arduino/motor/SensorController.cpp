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

void SensorController::printSensorFeedback() {
    unsigned char sfl = 48 + this->getIRGrids(Constants::IR_SHORT_FL);
    unsigned char sfm = 48 + this->getIRGrids(Constants::IR_SHORT_FM);
    unsigned char sfr = 48 + this->getIRGrids(Constants::IR_SHORT_FR);

    unsigned char sl = 48 + this->getIRGrids(Constants::IR_SHORT_L);
    unsigned char ll = 48 + this->getIRGrids(Constants::IR_LONG_L);

    unsigned char l = 48 + sl >= 4 ? ll : sl;

    unsigned char ull = 48 + this->getUl(Constants::UL1_PWM, Constants::UL1_TRIG);
    unsigned char ulr = 48 + this->getUl(Constants::UL2_PWM, Constants::UL2_TRIG);

    char output[8] = {'p', sfl, sfm, sfr, l, ull, ulr, '\0'};

    // c = 48 + this->getIRGrids(Constants::IR_LONG_F);
    // c = 48 + this->getIRGrids(Constants::IR_LONG_L);
    Serial.println(output);
    Serial.flush();
}

void SensorController::printSensorFeedbackCalibration() {
    // FL = 'A' + grid
    String output = String();

    output = output + "FL: " + this->getIRShortCM(Constants::IR_SHORT_FL) + " ";
    output = output + "FM: " + this->getIRShortCM(Constants::IR_SHORT_FM) + " ";
    output = output + "FR: " + this->getIRShortCM(Constants::IR_SHORT_FR) + " ";

    output = output + "SL: " + this->getIRShortCM(Constants::IR_SHORT_L) + " ";
    output = output + "LL: " + this->getIRShortCM(Constants::IR_LONG_L) + " ";

    output = output + "ULL: " + this->getUlCM(Constants::UL1_PWM, Constants::UL1_TRIG);
    output = output + "ULR: " + this->getUlCM(Constants::UL2_PWM, Constants::UL2_TRIG);

    Serial.println(output);
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    // TODO: ditch readings larger than x grids
    float offset = 0;
    uint8_t grids = 0;
    switch (pin) {
    case Constants::IR_SHORT_FL:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) + 2 + 4) / 10); // 5 for rounding
        return grids > 4 ? 9 : grids;
    case Constants::IR_SHORT_FM:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) + 1 + 4) / 10); // 5 for rounding
        return grids > 4 ? 9 : grids;
    case Constants::IR_SHORT_FR:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) + 2 + 4) / 10); // 5 for rounding
        return grids > 4 ? 9 : grids;
    case Constants::IR_SHORT_L:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) + 0 + 4) / 10); // 5 for rounding
        return grids > 4 ? 9 : grids;
    case Constants::IR_LONG_L:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) + 0 + 4) / 10); // 5 for rounding
        return grids > 4 ? 9 : grids;
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
    sum = analogRead(pin); // Ditched
    sum = analogRead(pin); // Ditched
    sum = analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    return (sum >> 2);
}

int SensorController::getUl(unsigned char ulPwm, unsigned char ulTrig) {
    if (this->getUlCM(ulPwm, ulTrig) > 15)
        return 1;
    else
        return 0;
}

float SensorController::getUlCM(unsigned char ulPwm, unsigned char ulTrig) {

    digitalWrite(ulTrig, LOW);
    digitalWrite(ulTrig, HIGH);

    unsigned long distance = pulseIn(ulPwm, LOW);

    if (distance >= 50000) {          // the reading is invalid.
        // Serial.print("Invalid");
    } else {
        return (float)distance / 50.0;       // every 50us low level stands for 1cm
    }
}