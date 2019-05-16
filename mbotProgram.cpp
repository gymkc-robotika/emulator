#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

MeRGBLed rgbled_7(7, 7==7?2:4);
MeLineFollower linefollower_2(2);

void motorRL(int l, int r) {
  motor_9.run((9) == M1 ? -(l) : (l));
  motor_10.run((10) == M1 ? -(r) : (r));
}


long start;
enum State {
	Init,
	Turn,
	Reverse,
	ReverseForAWhile
};

State state = Init;


void doInit() {
	start =  millis();
	state = Init;
	motorRL(100, 80);
}

void setup() {
	doInit();
}

long reverseStart;

void loop() {
	if (state == Init) {
	   int line = linefollower_2.readSensors();
	   if (line !=0) {
			motorRL(-80, -80);
			state = Reverse;
	   }
	   if (millis() > start + 10000) {
			motorRL(120, 150);
			state = Turn;
		}
	}
	if (state == Turn) {
	   int line = linefollower_2.readSensors();
	   if (line !=0) {
			motorRL(-80, -80);
			state = Reverse;
	   }
	}
	if (state == Reverse) {
	   int line = linefollower_2.readSensors();
	   if (line == 0) {
	   	  state = ReverseForAWhile;
	   	  reverseStart = millis();
	   }
	}
	if (state == ReverseForAWhile) {
		if (millis() > reverseStart + 2000)  {
			doInit();
		}
	}
	
}
