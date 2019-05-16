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

	if (dist < 5) {
		state = Collision;
		collisionTime = millis();
		motorRL(-80, -90);
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
	if (state == Following) {
		if (line == 0) {
			rgbled_7.setColor(0, 255, 128, 0);
			motorRL(-80, -85);
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
	if (state == Reverse) {
		if (line != 0) {
			state = Following;
			motorRL(150, 150);
		}
	}

}
