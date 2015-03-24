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
    pinMode(Constants::UL_LEFT_TRIG, OUTPUT);
    digitalWrite(Constants::UL_LEFT_TRIG, HIGH);
    pinMode(Constants::UL_RIGHT_TRIG, OUTPUT);
    digitalWrite(Constants::UL_RIGHT_TRIG, HIGH);

    pinMode(Constants::UL_LEFT_PWM, INPUT);
    pinMode(Constants::UL_RIGHT_PWM, INPUT);
}

void SensorController::printSensorFeedback() {
    unsigned char sfl = 48 + this->getIRGrids(Constants::IR_SHORT_FL);
    unsigned char sfm = 48 + this->getIRGrids(Constants::IR_SHORT_FM);
    unsigned char sfr = 48 + this->getIRGrids(Constants::IR_SHORT_FR);

    unsigned char sl = 48 + this->getIRGrids(Constants::IR_SHORT_L);
    unsigned char ll = 48 + this->getIRGrids(Constants::IR_LONG_L);

    unsigned char l = (sl >= 51 ? ll : sl);

    unsigned char ull = 48 + this->getUl(Constants::UL_LEFT_PWM, Constants::UL_LEFT_TRIG);
    unsigned char ulr = 48 + this->getUl(Constants::UL_RIGHT_PWM, Constants::UL_RIGHT_TRIG);

    unsigned char ul = ull > ulr ? ull : ulr;

    char output[7] = {'p', sfl, sfm, sfr, l, ul, '\0'};

    // c = 48 + this->getIRGrids(Constants::IR_LONG_F);
    // c = 48 + this->getIRGrids(Constants::IR_LONG_L);
    // Serial.println(output);

    Serial.print(output);
    // Serial.print(" IR SHORT: ");
    // Serial.print(this->getIRShortCM(Constants::IR_SHORT_FM));
    // Serial.print(" IR SHORT L: ");
    // Serial.print(this->getAnalogReading(Constants::IR_SHORT_FL));
    // Serial.print(" IR SHORT M: ");
    // Serial.print(this->getAnalogReading(Constants::IR_SHORT_FM));
    // Serial.print(" IR SHORT R: ");
    // Serial.print(this->getAnalogReading(Constants::IR_SHORT_FR));
    Serial.print(" UL Left: ");
    Serial.print(getUlCM(Constants::UL_LEFT_PWM, Constants::UL_LEFT_TRIG));
    Serial.print(" UL Right: ");
    Serial.print(getUlCM(Constants::UL_RIGHT_PWM, Constants::UL_RIGHT_TRIG));
    Serial.println();

    Serial.flush();
}

void SensorController::printSensorFeedbackCalibration() {
    delay(100);
    // FL = 'A' + grid
    String output = String();

    // output = output + "FL: " + this->getAnalogReading(Constants::IR_SHORT_FL) + " ";
    // output = output + "FM: " + this->getAnalogReading(Constants::IR_SHORT_FM) + " ";
    // output = output + "FR: " + this->getAnalogReading(Constants::IR_SHORT_FR) + " ";

    output = output + "SL: " + this->getAnalogReading(Constants::IR_SHORT_L) + " ";
    output = output + "LL: " + this->getAnalogReading(Constants::IR_LONG_L) + " ";

    // output = output + "FL: " + this->getIRShortCM(Constants::IR_SHORT_FL) + " ";
    // output = output + "FM: " + this->getIRShortCM(Constants::IR_SHORT_FM) + " ";
    // output = output + "FR: " + this->getIRShortCM(Constants::IR_SHORT_FR) + " ";

    // output = output + "SL: " + this->getIRShortCM(Constants::IR_SHORT_L) + " ";
    // output = output + "LL: " + this->getIRShortCM(Constants::IR_LONG_L) + " ";

    // output = output + "ULL: " + this->getUlCM(Constants::UL_LEFT_PWM, Constants::UL_LEFT_TRIG);
    // output = output + "ULR: " + this->getUlCM(Constants::UL_RIGHT_PWM, Constants::UL_RIGHT_TRIG);

    Serial.println(output);
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    int reading = this->getAnalogReading(pin);
    switch (pin) {
    case Constants::IR_SHORT_FL:
        if (reading > 415)
            return 1;
        else if (reading > 245)
            return 2;
        // else if (reading > 165)
        //     return 3;
        // else if (reading > 110)
        //     return 4;
        else
            return 9;
    case Constants::IR_SHORT_FM:
        if (reading > 450)
            return 1;
        else if (reading > 260)
            return 2;
        // else if (reading > 160)
        //     return 3;
        // else if (reading > 110)
        //     return 4;
        else
            return 9;
    case Constants::IR_SHORT_FR:
        if (reading > 410)
            return 1;
        else if (reading > 235)
            return 2;
        // else if (reading > 175)
        //     return 3;
        else
            return 9;
    case Constants::IR_SHORT_L:
        if (reading > 530)
            return 1;
        else if (reading > 265)
            return 2;
        else if (reading > 160)
            return 3;
        else if (reading > 120)
            return 4;
        else
            return 9;
    case Constants::IR_LONG_L:
        if (reading > 540)
            return 2;
        else if (reading > 415)
            return 3;
        else if (reading > 320)
            return 4;
        else if (reading > 245)
            return 5;
        // else if (reading > 190)
        //     return 6;
        else
            return 6;
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
    if (this->getUlCM(ulPwm, ulTrig) < 19)
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