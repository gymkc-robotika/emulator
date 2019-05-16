#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

MeRGBLed rgbled_7(7, 7==7?2:4);
MeLineFollower linefollower_2(2);

void setup() {
  motor_9.run((9) == M1 ? -(100) : (100));
  motor_10.run((10) == M1 ? -(80) : (80));
}

void loop() {
   int line = linefollower_2.readSensors();
   if (line !=0) {
      motor_9.run((9) == M1 ? -(-80) : (-80));
      motor_10.run((10) == M1 ? -(-80) : (-80));
   }
}
