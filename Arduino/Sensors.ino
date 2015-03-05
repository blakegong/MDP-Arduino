// # Description:
// # The Sketch for scanning 180 degree area 4-500cm detecting range

// # Connection:
// #       Pin 1 VCC (URM V3.2) -> VCC (Arduino)
// #       Pin 2 GND (URM V3.2) -> GND (Arduino)
// #       Pin 3 PWM (URM V3.2) -> Pin 3 (Arduino)
// #       Pin 5 COMP/TRIG (URM V3.2) -> Pin 5 (Arduino)
// #       Pin 4 PWM (URM V3.2) -> Pin 9 (Arduino)
// #       Pin 6 COMP/TRIG (URM V3.2) -> Pin 10 (Arduino)
// #
int SR1 = A2;
int SR2 = A3;
int LR1 = A4;
int LR2 = A5;
float sensorValue;
int CONSTANTDELAY = 500;
int URPWM1 = 3; // PWM Output 0?25000US?Every 50US represent 1cm
int URTRIG1 = 5; // PWM trigger pin
int URPWM2 = 9; // PWM Output 0?25000US?Every 50US represent 1cm
int URTRIG2 = 10; // PWM trigger pin
int RangeLow = 20;
int RangeHigh = 400;
int led = 13;

unsigned int URMDistance1 = 0;
unsigned int URMDistance2 = 0;
uint8_t EnPwmCmd[4] = {0x44, 0x02, 0xbb, 0x01}; // distance measure command

void setup() {          // Serial initialization
    pinMode(SR1, INPUT);
    pinMode (LR1, INPUT);
    pinMode(led, OUTPUT);
    Serial.begin(9600);                         // Sets the baud rate to 9600
    PWM_Mode_Setup();
}

void loop()
{
    PWM_Mode(); //URM
    delay(CONSTANTDELAY);
    float SR1distance = 12343.85 * pow(analogRead(SR1), -1.15); //calculate the distance in cm (SHORT RANGE)
    Serial.print("SR1Distance=");
    Serial.print(SR1distance);
    Serial.println("cm");
    delay(CONSTANTDELAY);

    float SR2distance = 12343.85 * pow(analogRead(SR2), -1.15); //calculate the distance in cm (SHORT RANGE)
    Serial.print("SR2Distance=");
    Serial.print(SR2distance);
    Serial.println("cm");
    delay(CONSTANTDELAY);

    sensorValue = analogRead(LR1);
    float LR1distance = 10650.08 * pow(sensorValue, -0.935) - 10; //calculate the distance in cm (LONG RANGE)
    Serial.print("LR1Distance=");
    Serial.print(LR1distance);
    Serial.println("cm");
    delay(CONSTANTDELAY);
}                      //PWM mode setup function

void PWM_Mode_Setup() {
    pinMode(URTRIG1, OUTPUT);                    // A low pull on pin COMP/TRIG
    digitalWrite(URTRIG1, HIGH);                 // Set to HIGH
    pinMode(URTRIG2, OUTPUT);                    // A low pull on pin COMP/TRIG
    digitalWrite(URTRIG2, HIGH);                 // Set to HIGH

    pinMode(URPWM1, INPUT);                      // Sending Enable PWM mode command
    pinMode(URPWM2, INPUT);                      // Sending Enable PWM mode command

    for (int i = 0; i < 4; i++) {
        Serial.write(EnPwmCmd[i]);
    }
}

void PWM_Mode() {                             // a low pull on pin COMP/TRIG  triggering a sensor reading
    digitalWrite(URTRIG1, LOW);
    digitalWrite(URTRIG1, HIGH);               // reading Pin PWM will output pulses
    digitalWrite(URTRIG2, LOW);
    digitalWrite(URTRIG2, HIGH);               // reading Pin PWM will output pulses


    unsigned long DistanceMeasured1 = pulseIn(URPWM1, LOW);
    unsigned long DistanceMeasured2 = pulseIn(URPWM2, LOW);

    if (DistanceMeasured1 == 50000) {          // the reading is invalid.
        Serial.print("Invalid");
    }
    else {
        URMDistance1 = DistanceMeasured1 / 50;       // every 50us low level stands for 1cm
    }
    Serial.print("Left URM Distance=");
    Serial.print(URMDistance1);
    Serial.println("cm");

    if ((URMDistance1 > RangeLow) && (URMDistance1 < RangeHigh))
    {
        digitalWrite(led, HIGH);
    }
    else
    {
        digitalWrite(led, LOW);
    }

    if (DistanceMeasured2 == 50000) {          // the reading is invalid.
        Serial.print("Invalid");
    }
    else {
        URMDistance2 = DistanceMeasured2 / 50;       // every 50us low level stands for 1cm
    }
    Serial.print("Right URM Distance=");
    Serial.print(URMDistance2);
    Serial.println("cm");

    if ((URMDistance2 > RangeLow) && (URMDistance2 < RangeHigh))
    {
        digitalWrite(led, HIGH);
    }
    else
    {
        digitalWrite(led, LOW);
    }
}
