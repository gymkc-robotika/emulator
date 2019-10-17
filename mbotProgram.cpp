#include <float.h>

#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

class Memory {
	static const int maxSize = 64;

	double values[maxSize];
	long time[maxSize];
	int count = 0;

	void shift() {
		for (int i = 1; i <count; i++) {
			values[i - 1] = values[i];
			time[i - 1] = time[i];
		}
		count -= 1;
	}

	int findTime(long timeToFind) {
		for (int i = 1; i <count; i++) {
			if (time[i] >= timeToFind) return i;
		}
		return count;
	}

	public:
	void add(double value, long now) {
		if (count >= maxSize) {
			shift();
		}
		values[count] = value;
		time[count] = now;
		count ++;
	}

	double average(long now, long age) {
		int index = findTime(now - age);
		if (index >= count) return 0;
		double sum = 0;
		for (int i = index; i < count; i++) {
			sum += values[i];
		}
		return sum / (count - index);
	}

	double maximum(long now, long age) {
		int index = findTime(now - age);
		double result = -DBL_MAX;
		for (int i = index; i < count; i++) {
			result = max(result, values[i]);
		}
		return result;
	}

	double minimum(long now, long age) {
		int index = findTime(now - age);
		double result = DBL_MAX;
		for (int i = index; i < count; i++) {
			result = min(result, values[i]);
		}
		return result;
	}

	void reset() {
		count = 0;
	}
};


MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

MeRGBLed rgbled_7(7, 7 == 7 ? 2 : 4);
MeLineFollower linefollower_2(2);

MeUltrasonicSensor ultrasonic_3(3);

void motorRL(int l, int r) {
	int maxMotor = +255;
	int minMotor = -255;
	l = max(minMotor, min(l, maxMotor));
	r = max(minMotor, min(r, maxMotor));
	motor_9.run((9) == M1 ? -(l) : (l));
	motor_10.run((10) == M1 ? -(r) : (r));
}


long start;
enum State {
	Init,
	Following,
	Stop,
	Reverse,
	Collision
};

State state = Init;
long collisionTime;
bool buttonState = false;

double turn = 0;
double speed = 0;

int lastMillis = millis();

Memory turnMemory;

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
	motorRL(0, 0);
	rgbled_7.setColor(0, 255, 0, 0);
	rgbled_7.show();
}

void setup() {
	doInit();
}

void loop() {
	int now = millis();
	int deltaT = now - lastMillis;
	lastMillis = now;

	double deltaTurn = deltaT * 5.0;
	double deltaSpeed = deltaT * 3.0;


	int line = linefollower_2.readSensors();
	int dist = ultrasonic_3.distanceCm();

	if (state == Stop && buttonEdge() == ButtonReleased) {
		state = Init;
	}

	if (state == Init && buttonEdge() == ButtonReleased) {
		state = Following;
		turnMemory.reset();
	}
	if (state == Following) {
		if (buttonEdge() == ButtonPressed) {
			state = Stop;
			motorRL(0,0);
			turn = 0;
		}
	}

	if (false) {
		if (dist < 5) {
			state = Collision;
			collisionTime = millis();
			motorRL(-80, -90);
		}
	}

	if (state == Reverse) {
		// when reversing wait until both a black
		if (line == 0) {
			rgbled_7.setColor(0, 255, 128, 0);
			motorRL(0, 0);
			turn = 0;
			speed = 70;
			state = Following;
		}
	}

	if (state == Following) {
		double avgTurn = turnMemory.average(now, 1000);
		if (line == 3) {
			rgbled_7.setColor(0, 255, 128, 0);
			motorRL(-85, -85);
			turn = 0;
			speed = -80;
			state = Reverse;
		} else {
			if (line == 2) {
				turn = avgTurn + deltaTurn;
				if (turn > 512) turn = 512;
				speed -= deltaSpeed;
				if (speed < 0) speed = 0;
				rgbled_7.setColor(1, 255, 0, 0);
				rgbled_7.setColor(2, 200, 200, 0);
				rgbled_7.show();
			}
			if (line == 1) {
				turn = avgTurn - deltaTurn;
				if (turn < -512) turn = -512;
				speed -= deltaSpeed;
				if (speed < 0) speed = 0;
				rgbled_7.setColor(1, 200, 200, 0);
				rgbled_7.setColor(2, 255, 0, 0);
				rgbled_7.show();
			}
			if (line == 0) {
				speed += deltaSpeed;
				if (speed < 70) speed = 70;
				if (speed > 200) speed = 200;
				turn = avgTurn;
				rgbled_7.setColor(0, 150, 150, 0);
				rgbled_7.show();
			}

			turnMemory.add(turn, now);

			int motorR = speed + turn;
			int motorL = speed - turn;
			motorRL(motorR, motorL);
		}
	}
}

