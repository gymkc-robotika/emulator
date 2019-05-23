#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

MeRGBLed rgbled_7(7, 7 == 7 ? 2 : 4);
MeLineFollower linefollower_2(2);

MeUltrasonicSensor ultrasonic_3(3);

void motorRL(int l, int r) {
	motor_9.run((9) == M1 ? -(l) : (l));
	motor_10.run((10) == M1 ? -(r) : (r));
}


long start;
enum State {
	Init,
	Following,
	Reverse,
	Collision
};

State state = Init;
long collisionTime;
bool buttonState = false;

#define ButtonNone 0
#define ButtonPressed 1
#define ButtonReleased 2

int buttonEdge() {
  int ret = ButtonNone;
  bool button = analogRead(A7) < 10;
  if (button && !buttonState) {
    ret = ButtonPressed;
  }
  if (!button && buttonState) {
    ret = ButtonReleased;
  }
  buttonState = button;
  return ret;
}


void doInit() {
	state = Init;
	rgbled_7.setColor(0, 0, 0, 0);
	rgbled_7.show();
}

void setup() {
	doInit();
}

void loop() {
	if (state == Init) {
		if (buttonEdge() == ButtonReleased) {
			state = Following;
			motorRL(100, 100);
			rgbled_7.setColor(0, 255, 0, 0);
			rgbled_7.show();
		}
	}
	if (state == Following) {
		if (buttonEdge() == ButtonPressed) {
			state = Collision;
			motorRL(0, 0);
			rgbled_7.setColor(0, 0, 0, 0);
			rgbled_7.show();
		}
	}
	if (state == Collision) {
		if (buttonEdge() == ButtonPressed) {
			state = Init;
		}
		
	}
}

