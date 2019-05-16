#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

void setup() {
  motor_9.run((9) == M1 ? -(100) : (100));
  motor_10.run((10) == M1 ? -(80) : (80));
}

void loop() {

}
