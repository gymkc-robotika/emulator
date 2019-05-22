#include <iostream>
#include <cmath>
#include <windows.h>
#include "mBotEmul.h"

#include "MeMCore.h"

class MBot : public MBotPos, MBotConfig {
	MeDCMotor *motorR;
	MeDCMotor *motorL;

	public:
	COLORREF ledLeftSet, ledRightSet;

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
	double speedR = motorR->effectiveSpeed();
	double speedL = motorL->effectiveSpeed();

	auto frontLeft = GetRoomColor(local(collisionL, collisionFront));
	auto frontRight = GetRoomColor(local(collisionR, collisionFront));

	// obstacle in front means we cannot move forward
	if (frontLeft == RoomWall) {
		speedL = std::min(speedL, 0.0);
	}
	if (frontRight == RoomWall) {
		speedR = std::min(speedR, 0.0);
	}

	double speed = (speedR + speedL) * 0.001;
	double turnCoef = 0.01;
	double headingChange = (speedR - speedL) * turnCoef;

	heading += headingChange * dt;

	pos.x += speed * dt * sin(heading);
	pos.y += speed * dt * cos(heading);

	sensorLeft = GetRoomColor(local(lineSensorPosL, lineSensorPosFront));
	sensorRight = GetRoomColor(local(lineSensorPosR, lineSensorPosFront));

	ultrasonicDistance = std::min(RoomRayCast(local(0, botScale * 1.1), local(0, botScale * 1.1 + 4.1)), 4.0);
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

int MeUltrasonicSensor::distanceCm() {
	return int(ceil(bot.ultrasonicDistance * 100));
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

void placeMBot(double xPos, double yPos) {
	bot.pos.x = xPos;
	bot.pos.y = yPos;
}

void rotateMBot(double heading) {
	bot.heading = heading;
}


void MeRGBLed::setColor(int leftRight, int r, int g, int b) {
	{
		if (leftRight == 0 || leftRight == 2) {
			bot.ledLeftSet = RGB(r, g, b);
		}
		if (leftRight == 0 || leftRight == 1) {
			bot.ledRightSet = RGB(r, g, b);
		}
	}
}

void MeRGBLed::show() {
	bot.ledLeft = bot.ledLeftSet;
	bot.ledRight = bot.ledRightSet;
}
