#include <iostream>
#include <cmath>
#include <windows.h>
#include "mBotEmul.h"

#include "MeMCore.h"

class MBot : public MBotPos {
	MeDCMotor *motorR;
	MeDCMotor *motorL;

	RoomColor sensorLeft = RoomWhite, sensorRight = RoomWhite;

	public:
	MBot(MeDCMotor *rm, MeDCMotor *lm) {
		motorR = rm;
		motorL = lm;
	}

	// current speed
	double speedX = 0, speedY = 0;

	void move(double dt);

	RoomColor SensorLColor() {return sensorLeft;}
	RoomColor SensorRColor() {return sensorRight;}

};

void MBot::move(double dt) {
	// TODO: inertia / acceleration
	double speed = (motorR->speed + motorL->speed) * 0.001;
	double turnCoef = 0.01;
	double headingChange = (motorR->speed - motorL->speed) * turnCoef;

	heading += headingChange * dt;

	pos.x += speed * dt * sin(heading);
	pos.y += speed * dt * cos(heading);

	sensorLeft = GetRoomColor(local(+0.01, 0.1));
	sensorRight = GetRoomColor(local(-0.01, 0.1));
}


MBot bot = MBot(&motor_9, &motor_10);

int MeLineFollower::readSensors() {
	RoomColor colorR = bot.SensorLColor();
	RoomColor colorL = bot.SensorRColor();
	int ret = 0;
	if (colorR == RoomBlack) ret += 1;
	if (colorL == RoomBlack) ret += 2;
	return ret;

}

MBotPos getVisual() {
	return bot;
}

MBotPos emulatorSetup() {
	setup();
	return getVisual();
}

MBotPos emulatorLoop(double dt) {
	loop();
	bot.move(dt);
	return getVisual();
}


