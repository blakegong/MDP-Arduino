#include <SensorController.h>
#include "Constants.h"

SensorController::SensorController() {
    initIR();
    initServo();
    servoDirection = Constants::DIRECT_L;
}

void SensorController::initIR() {
    pinMode(Constants::IRS_FL, INPUT);
    pinMode(Constants::IRS_FM, INPUT);
    pinMode(Constants::IRS_FR, INPUT);
    pinMode(Constants::IRS_F, INPUT);
    pinMode(Constants::IRS_F, INPUT);
}

void SensorController::initServo() {
    // this->servo.attach(5);
}

void SensorController::printSensorFeedback() {
    unsigned char sf, l, f_l, f_r = 0;
    unsigned char sfl = this->getIRGrids(Constants::IRS_FL);
    unsigned char sfm = this->getIRGrids(Constants::IRS_FM);
    unsigned char sfr = this->getIRGrids(Constants::IRS_FR);
    unsigned char sr = this->getIRGrids(Constants::IRS_R);

    if (this->servoDirection == Constants::DIRECT_L) {
        sf = this->getIRGrids(Constants::IRS_F);
        l = this->getIRGrids(Constants::IRL);
        f_l = (sf >= 3 ? l : sf);
        if (sr > 1) {
            this->setServo(Constants::DIRECT_R);
            sf = this->getIRGrids(Constants::IRS_F);
            l = this->getIRGrids(Constants::IRL);
            f_r = (sf >= 3 ? l : sf);
        }
    } else {
        if (sr > 1) {
            sf = this->getIRGrids(Constants::IRS_F);
            l = this->getIRGrids(Constants::IRL);
            f_r = (sf >= 3 ? l : sf);
        }
        this->setServo(Constants::DIRECT_L);
        sf = this->getIRGrids(Constants::IRS_F);
        l = this->getIRGrids(Constants::IRL);
        f_l = (sf >= 3 ? l : sf);
    }

    char output[7] = {'p', 48 + sfl, 48 + sfm, 48 + sfr, 48 + f_l, 48 + (f_r > 0 ? f_r : sr), '\0'};
    Serial.print(output);

    Serial.println();
    Serial.flush();
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    int reading = this->getAnalogReading(pin);
    switch (pin) {
    case Constants::IRS_FL:
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
    case Constants::IRS_FM:
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
    case Constants::IRS_FR:
        if (reading > 410)
            return 1;
        else if (reading > 235)
            return 2;
        // else if (reading > 175)
        //     return 3;
        else
            return 9;
    case Constants::IRS_F:
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
    case Constants::IRS_R:
        if (reading > 530)
            return 1;
    case Constants::IRL:
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
    sum = analogRead(pin); // Ditched
    sum = analogRead(pin); // Ditched
    sum = analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    sum += analogRead(pin);
    return (sum >> 3);
}

void SensorController::setServo(unsigned char direction) {
    // switch (direction) {
    // case Constants::DIRECT_F:
    //     servo.write(90);
    //     servo.write(90);
    //     break;
    // case Constants::DIRECT_L:
    //     servo.write(0);
    //     servo.write(0);
    //     break;
    // case Constants::DIRECT_R:
    //     servo.write(180);
    //     servo.write(180);
    //     break;
    // }
    this->servoDirection = direction;
}