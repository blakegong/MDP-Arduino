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
    pinMode(Constants::IRS, INPUT);
    pinMode(Constants::IRL, INPUT);
}

void SensorController::initServo() {
    this->servo.attach(Constants::SERVO);
}

void SensorController::printSensorFeedback() {
    unsigned char l, r, irs, irl;
    unsigned char sfl = this->getIRGrids(Constants::IRS_FL);
    unsigned char sfm = this->getIRGrids(Constants::IRS_FM);
    unsigned char sfr = this->getIRGrids(Constants::IRS_FR);
    if (this->isServoRead) {
        if (this->servoDirection == Constants::DIRECT_L) {
            this->setServo(Constants::DIRECT_L);
            delay(10);
            irs = this->getIRGrids(Constants::IRS);
            irl = this->getIRGrids(Constants::IRL);
            l = irs > 1 ? irl : irs;
            this->setServo(Constants::DIRECT_R);
            delay(750);
            irs = this->getIRGrids(Constants::IRS);
            irl = this->getIRGrids(Constants::IRL);
            r = irs > 1 ? irl : irs;
        } else {
            this->setServo(Constants::DIRECT_R);
            delay(10);
            irs = this->getIRGrids(Constants::IRS);
            irl = this->getIRGrids(Constants::IRL);
            r = irs > 1 ? irl : irs;
            this->setServo(Constants::DIRECT_L);
            delay(750);
            irs = this->getIRGrids(Constants::IRS);
            irl = this->getIRGrids(Constants::IRL);
            l = irs > 1 ? irl : irs;
        }
    } else {
        l = 8;
        r = 8;
    }

    char output[7] = {'p', 48 + sfl, 48 + sfm, 48 + sfr, 48 + l, 48 + r, '\0'};
    Serial.print(output);

    if (servoDirection == Constants::DIRECT_L)
        Serial.print(" Servo: L");
    else
        Serial.print(" Servo: R");

}

void SensorController::printSensorRawData() {
    Serial.print("FL: ");
    Serial.print(this->getAnalogReading(Constants::IRS_FL));
    Serial.print(" FM: ");
    Serial.print(this->getAnalogReading(Constants::IRS_FM));
    Serial.print(" FR: ");
    Serial.print(this->getAnalogReading(Constants::IRS_FR));
    Serial.print(" Servo S: ");
    Serial.print(this->getAnalogReading(Constants::IRS));
    Serial.print(" Servo L: ");
    Serial.print(this->getAnalogReading(Constants::IRL));
    Serial.println();
}

unsigned char SensorController::getIRGrids(unsigned char pin) {
    int reading = this->getAnalogReading(pin);
    switch (pin) {
    case Constants::IRS_FL:
        if (reading > 373)
            return 1;
        else if (reading > 225)
            return 2;
        else
            return 9;
    case Constants::IRS_FM:
        if (reading > 384)
            return 1;
        else if (reading > 225)
            return 2;
        else
            return 9;
    case Constants::IRS_FR:
        if (reading > 377)
            return 1;
        else if (reading > 230)
            return 2;
        else
            return 9;
    case Constants::IRS:
        if (reading > 247)
            return 1;
        else if (reading > 155)
            return 2;
        else
            return 9;
    case Constants::IRL:
        if (reading > 404)
            return 2;
        else if (reading > 310)
            return 3;
        else if (reading > 245)
            return 4;
        else if (reading > 205)
            return 5;
        // else if (reading > 160)
        //     return 6;
        else
            return 9;
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
    switch (direction) {
    case Constants::DIRECT_L:
        servo.write(9999);
        servo.write(9999);
        break;
    case Constants::DIRECT_R:
        servo.write(0);
        servo.write(0);
        break;
    }
    this->servoDirection = direction;
}

void SensorController::setServoRead(bool isServoRead) {
    this->isServoRead = isServoRead;
}