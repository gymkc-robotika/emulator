#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

MeRGBLed rgbled_7(7, 7 == 7 ? 2 : 4);
MeLineFollower linefollower_2(2);

void motorRL(int l, int r) {
	motor_9.run((9) == M1 ? -(l) : (l));
	motor_10.run((10) == M1 ? -(r) : (r));
}


long start;
enum State {
	Init,
	Following,
	Reverse,
};

State state = Init;


void doInit() {
	start = millis();
	state = Init;
	motorRL(100, 100);
}

void setup() {
	doInit();
}

void loop() {
	int line = linefollower_2.readSensors();
	if (state == Init) {
		if (line != 0) {
			motorRL(150, 150);
			state = Following;
		}
	}
	if (state == Following) {
		if (line == 0) {
			motorRL(-80, -85);
			state = Reverse;
		}
		if (line == 1) {
			motorRL(+80, -80);
		}
		if (line == 2) {
			motorRL(-80, +80);
		}
		if (line == 3) {
			motorRL(150, 150);
		}
	}
	if (state == Reverse) {
		if (line != 0) {
			state = Following;
			motorRL(150, 150);
		}
	}

}
