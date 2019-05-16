#include <iostream>
#include <cmath>
#include "mBotEmul.h"

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
};


class MBot {
  MeDCMotor *motorR;
  MeDCMotor *motorL;

  public:
  MBot(MeDCMotor *rm, MeDCMotor *lm) {
    motorR = rm;
    motorL = lm;
  }

  // coordinates
  double x = 0, y = 0;
  double heading = 0;
  // current speed
  double speedX = 0, speedY = 0;

  void move(double dt) {
    // TODO: inertia / acceleration
    double speed = (motorR->speed + motorL->speed) * 0.001;
    double turnCoef = 0.01;
    double headingChange = (motorR->speed - motorL->speed) * turnCoef;

    heading += headingChange * dt;

    x += speed * dt * sin(heading);
    y += speed * dt * cos(heading);
  }
};


// declare imports - expect the mBot sketch
void loop();

void setup();

extern MeDCMotor motor_9;
extern MeDCMotor motor_10;
MBot bot = MBot(&motor_9, &motor_10);

mBotVisual getVisual() {
   mBotVisual visual;
   visual.x = bot.x;
   visual.y = bot.y;
   visual.heading = bot.heading;
   return visual;
}

mBotVisual emulatorSetup() {
   printf("Emulator started - coordinates %.3f,%.3f\n", bot.x, bot.y);
   setup();
   printf("Setup done\n");
   return getVisual();
}

mBotVisual emulatorLoop(double dt) {
   loop();
   bot.move(dt);
   return getVisual();
}

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

void setup() {
  motor_9.run((9) == M1 ? -(100) : (100));
  motor_10.run((10) == M1 ? -(80) : (80));
}

void loop() {

}
