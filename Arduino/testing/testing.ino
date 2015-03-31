#include "Arduino.h"
#include <Servo.h>

Servo myservo;

int pos = 0;
void setup()
{
    Serial.begin(9600);
    Serial.setTimeout(2);
    myservo.attach(9, 580, 2340);
}

void loop() {
    // myservo.write(140);
    // delay(50);
    // myservo.write(95);
    // delay(10);
    // myservo.write(120);
    // delay(300);
    // myservo.writeMicroseconds(1480);
    myservo.write(90);
    delay(1500);
    myservo.write(10);
    delay(10);
    myservo.write(0);
    delay(1500);
    myservo.write(170);
    delay(10);
    myservo.write(180);
    delay(1500);
    // for (pos = 0; pos < 180; pos += 1) // goes from 0 degrees to 180 degrees
    // {   // in steps of 1 degree
    //     myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //     delay(15);                       // waits 15ms for the servo to reach the position
    // }
    // for (pos = 180; pos >= 1; pos -= 1) // goes from 180 degrees to 0 degrees
    // {
    //     myservo.write(pos);              // tell servo to go to position in variable 'pos'
    //     delay(15);                       // waits 15ms for the servo to reach the position
    // }
}