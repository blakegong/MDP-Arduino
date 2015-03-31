//Test Sketch using ServoTimer2 library to drive a single standard servo attached to pin 5. write function takes an input of the pulsewidth (NOT ANGLE).
//Author: DM

#include <ServoTimer2.h>  // the servo library

ServoTimer2 servo;    // declare variables for up to eight servos
 
void setup() {
    servo.attach(5);
}

void loop()
{ 
      servo.write(2000);
      delay(1000);
      servo.write(0);
      delay(1000);
}
