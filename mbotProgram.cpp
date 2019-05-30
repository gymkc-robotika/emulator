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
	start = millis();
	state = Init;
	motorRL(100, 100);
	rgbled_7.setColor(0, 255, 0, 0);
	rgbled_7.show();
}

void setup() {
	doInit();
}

void loop() {
	int line = linefollower_2.readSensors();
	int dist = ultrasonic_3.distanceCm();

	if (buttonEdge() == ButtonPressed) {
		doInit();
	}

	if (false) {
		if (dist < 5) {
			state = Collision;
			collisionTime = millis();
			motorRL(-80, -90);
		}
	}

	if (state == Collision) {
		if (millis() > collisionTime + 3000) {
			doInit();
		}
	}
	if (state == Init) {
		if (line != 0) {
			motorRL(150, 150);
			state = Following;
		}
	}
	if (state == Reverse && line == 0) {
		rgbled_7.setColor(0, 255, 128, 0);
		motorRL(-85, -85);
	}
	if (state == Following || (state == Reverse && line != 0)) {
		if (line == 0) {
			rgbled_7.setColor(0, 255, 128, 0);
			motorRL(-85, -85);
			state = Reverse;
		}
		if (line == 1) {
			motorRL(+80, -80);
			rgbled_7.setColor(1, 255, 0, 0);
			rgbled_7.setColor(2, 200, 200, 0);
			rgbled_7.show();
		}
		if (line == 2) {
			motorRL(-80, +80);
			rgbled_7.setColor(1, 200, 200, 0);
			rgbled_7.setColor(2, 255, 0, 0);
			rgbled_7.show();
		}
		if (line == 3) {
			motorRL(150, 150);
			rgbled_7.setColor(0, 150, 150, 0);
			rgbled_7.show();
		}
	}
}

