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

    char output[9] = {'b', sfl, sfm, sfr, '4', '4', '1', '1', '\0'};

    // c = 48 + this->getIRGrids(Constants::IR_LONG_F);
    // c = 48 + this->getIRGrids(Constants::IR_LONG_L);
    Serial.write(output);
}

void SensorController::printSensorFeedbackCalibration() {
    // FL = 'A' + grid
    String output = String();

    output = output + "FL: " + this->getIRShortCM(Constants::IR_SHORT_FL) + " ";
    output = output + "FM: " + this->getIRShortCM(Constants::IR_SHORT_FM) + " ";
    Serial.println(output);
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    // TODO: ditch readings larger than x grids
    float offset = 0;
    uint8_t grids = 0;
    switch (pin) {
    case Constants::IR_SHORT_FL:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) - offset + 5) / 10); // 5 for rounding
        return grids > 4 ? 'O' : grids;
    case Constants::IR_SHORT_FM:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) - offset + 5) / 10); // 5 for rounding
        return grids > 4 ? 'O' : grids;
    case Constants::IR_SHORT_FR:
        offset = 0;
        grids = (unsigned char) ((getIRShortCM(pin) - offset + 5) / 10); // 5 for rounding
        return grids > 4 ? 'O' : grids;
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