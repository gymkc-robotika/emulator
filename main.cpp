#include <iostream>
#include <cmath>
#include <windows.h>
#include "mBotEmul.h"

#include "MeMCore.h"

class MBot : public MBotPos {
	MeDCMotor *motorR;
	MeDCMotor *motorL;

	public:
	MBot(MeDCMotor *rm, MeDCMotor *lm) {
		motorR = rm;
		motorL = lm;
	}

	// current speed
	double speedX = 0, speedY = 0;

	void move(double dt);
};

void MBot::move(double dt) {
	// TODO: inertia / acceleration
	double speed = (motorR->effectiveSpeed() + motorL->effectiveSpeed()) * 0.001;
	double turnCoef = 0.01;
	double headingChange = (motorR->effectiveSpeed() - motorL->effectiveSpeed()) * turnCoef;

	auto frontLeft = GetRoomColor(local(+0.03, 0.12));
	auto frontRight = GetRoomColor(local(-0.05, 0.12));

	// obstacle in front means we cannot move forward
	if (frontLeft == RoomWall || frontRight == RoomWall) {
		speed = std::min(speed, 0.0);
	}

	heading += headingChange * dt;

	pos.x += speed * dt * sin(heading);
	pos.y += speed * dt * cos(heading);

	sensorLeft = GetRoomColor(local(+0.01, 0.1));
	sensorRight = GetRoomColor(local(-0.01, 0.1));
}


MBot bot = MBot(&motor_9, &motor_10);

int MeLineFollower::readSensors() {
	RoomColor colorR = bot.sensorLeft;
	RoomColor colorL = bot.sensorRight;
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


