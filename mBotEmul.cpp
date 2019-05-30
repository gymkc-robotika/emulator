#include <iostream>
#include <cmath>
#include <windows.h>
#include <ctime>
#include "mBotEmul.h"

#include "MeMCore.h"

class MBot : public MBotPos, MBotConfig {
	MeDCMotor *motorR;
	MeDCMotor *motorL;

public:
	COLORREF ledLeftSet, ledRightSet;
	bool buttonState = false;

	MBot(MeDCMotor *rm, MeDCMotor *lm) {
		motorR = rm;
		motorL = lm;
	}

	// current speed
	double speedX = 0, speedY = 0;


	void move(double dt, bool button);
};


/**
 * Speed measurement results:
 * (L, R engine settings): time - description
	(-100, 100): 28.6 - 10x turn 360 degree
	(0, 100) 27.8 - 5x turn 360 deg (diameter about 10 cm => traveled about 5x3x10 = 150 cm)
	(-255,255): 9.6 - 10x turn 360 degree
	(100,100): 8.5 - travel 130 cm
	(255,255): 6.7 - travel 260 cm
	(-255,-255): 6.6 - travel 260 cm
	(100,200): 29.0 - 5x turn 360 deg (diameter about 50 cm => traveled about 5x3x50 = 750 cm)
 * */

/**
 	* Engine / speed table:
 	* engine : distance : time : speed, speed / engine
	50 : 1.5 : 27.8 - 0.054, 0.00108
	100: 1.3 : 8.5 - 0.153, 0.00153
	255: 2.6 : 6.7 - 0.388, 0.00152
	-255: 2.6: 6.6 - 0.394, 0.00154
	150: 29.0: 7.5 - 0.259, 0.00173
 ---
 	average speed / engine = 0.00148
*/

/**
  * Engine / turn table:
  * engine: turns: time: turn period, turn period * engine
  200: 10: 28.6: 2.86, 572
  510: 10: 9.6: 0.96, 490
  100: 5: 27.8:  5.56, 556
  100: 5: 29: 5.8, 580
 ---
  average turn period * engine = 550
  turn speed = 2*pi / turn period
  turn period = 2*pi / turn speed

  engine / turn speed = 550 / (2*pi)
  engine / turn speed = (2*pi) / 550
*/

void MBot::move(double dt, bool button) {
	buttonState = button;

	// TODO: inertia / acceleration
	motorR->simulate(dt);
	motorL->simulate(dt);

	double speedR = motorR->effectiveSpeed();
	double speedL = motorL->effectiveSpeed();

	auto frontLeft = GetRoomColor(local(collisionL, collisionFront));
	auto frontRight = GetRoomColor(local(collisionR, collisionFront));

	auto backLeft = GetRoomColor(local(collisionBL, collisionBack));
	auto backRight = GetRoomColor(local(collisionBR, collisionBack));

	// obstacle in front means we cannot move forward
	if (frontLeft == RoomWall) {
		speedL = std::min(speedL, 0.0);
	}
	if (frontRight == RoomWall) {
		speedR = std::min(speedR, 0.0);
	}

	if (backLeft == RoomWall) {
		speedL = std::max(speedL, 0.0);
	}
	if (backRight == RoomWall) {
		speedR = std::max(speedR, 0.0);
	}

	double speedCoef = 0.00148;
	double speed = (speedR + speedL) * speedCoef;
	double turnCoef = 0.011;
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
	int ret = 3;
	if (colorR == RoomBlack) ret -= 1;
	if (colorL == RoomBlack) ret -= 2;
	return ret;

}

int MeUltrasonicSensor::distanceCm() {
	return int(ceil(bot.ultrasonicDistance * 100));
}

MBotPos getVisual() {
	return bot;
}

MBotPos emulatorSetup() {
	srand(time(NULL));
	setup();
	return getVisual();
}

MBotPos emulatorLoop(double dt, bool button) {
	loop();
	bot.move(dt, button);
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

int analogRead(int pin) {
	return bot.buttonState ? 0 : 20;
}
