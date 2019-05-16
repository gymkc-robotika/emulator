#ifndef MEM_CORE_H
#define MEM_CORE_H

#include <math.h>

#define M1 0

class MeDCMotor {
	public:
	int motorNumber;
	int speed;

	explicit MeDCMotor(int number) {
		motorNumber = number;
		speed = 0;
	}

	void run(int s) {
		speed = s;
	}

	int effectiveSpeed() const {
		if (abs(speed) < 80) return 0;
		if (speed > 255) return 255;
		if (speed < - 255) return -255;
		return speed;
	}
};

class MeRGBLed {
	public:
	int myNumber;

	explicit MeRGBLed(int number, int dummy) {
		myNumber = number;
	}

	void setColor(int leftRight, int r, int g, int b) {
	}

	void show() {
	}
};

class MeLineFollower {
	public:
	explicit MeLineFollower(int port) {
	}

	int readSensors();
};

class MeUltrasonicSensor {
	public:
	explicit MeUltrasonicSensor(int port) {
	}

	int distanceCm();
};
#define A7 0xa000 + 7

static void pinMode(int pin, int mode) {
}

static int analogRead(int pin) {
	return 0;
}

long millis();


// declare imports - expect the mBot sketch
void loop();

void setup();

extern MeDCMotor motor_9;
extern MeDCMotor motor_10;

extern MeRGBLed rgbled_7;
extern MeLineFollower linefollower_2;

#endif

