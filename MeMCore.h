#ifndef MEM_CORE_H
#define MEM_CORE_H

#include <math.h>
#include <random>

#define M1 0
#define INPUT 0
#define A7 (0xa000 + 7)

class MeDCMotor {
	double randomFactor = 1;
	double randomDT = 0;
	static constexpr double randomDTAfter = 1.0;

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

	double effectiveSpeed() const {
		if (abs(speed) < 80) return 0;
		if (speed > 255) return 255 * randomFactor;
		if (speed < - 255) return -255 * randomFactor;
		return speed * randomFactor;
	}

	void simulate(double dt) {
		randomDT += dt;
		if (randomDT >= randomDTAfter) {
			randomDT -= randomDTAfter;
			double f = (rand() & RAND_MAX) / double(RAND_MAX);
			const double randomness = 0.02;
			randomFactor = 1 + f * randomness - randomness / 2;
		}
	}
};

class MeRGBLed {
	public:
	int myNumber;

	explicit MeRGBLed(int number, int dummy) {
		myNumber = number;
	}

	void setColor(int leftRight, int r, int g, int b);

	void show();
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

static void pinMode(int pin, int mode) {
}

int analogRead(int pin);

long millis();


// declare imports - expect the mBot sketch
void loop();

void setup();

extern MeDCMotor motor_9;
extern MeDCMotor motor_10;

extern MeRGBLed rgbled_7;
extern MeLineFollower linefollower_2;

#endif

