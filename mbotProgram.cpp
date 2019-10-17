#include "Arduino.h"
#include "Wire.h"
#include "SoftwareSerial.h"

#include "MeMCore.h"

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

void motorRL(int l, int r) {
	int maxMotor = +255;
	int minMotor = -255;
	l = std::max(minMotor, std::min(l, maxMotor));
	r = std::max(minMotor, std::min(r, maxMotor));
	motor_9.run((9) == M1 ? -(l) : (l));
	motor_10.run((10) == M1 ? -(r) : (r));
}


double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
double line;
double ultra;
MeRGBLed rgbled_1(1, 1==7?2:4);
MeLineFollower linefollower_2(2);
MeUltrasonicSensor ultrasonic_3(3);


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


void setup(){
	pinMode(A7,INPUT);
	while(!((0^(analogRead(A7)>10?0:1))))
	{
		_loop();
	}
	rgbled_1.setColor(0,0,0,0);
	rgbled_1.show();
	line = 0;
	ultra = 0;

}

void loop(){

	line = linefollower_2.readSensors();
	ultra = ultrasonic_3.distanceCm();

	_loop();
}

void _delay(float seconds){
	long endTime = millis() + seconds * 1000;
	while(millis() < endTime)_loop();
}

void _loop(){

}
