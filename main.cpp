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

   void move(double dt) {
      // TODO: inertia / acceleration
      double speed = (motorR->speed + motorL->speed) * 0.001;
      double turnCoef = 0.01;
      double headingChange = (motorR->speed - motorL->speed) * turnCoef;

      heading += headingChange * dt;

      pos.x += speed * dt * sin(heading);
      pos.y += speed * dt * cos(heading);
   }
};


MBot bot = MBot(&motor_9, &motor_10);

int MeLineFollower::readSensors() {
   RoomColor colorR = GetRoomColor(bot.pos); // TODO: read two sensors
   RoomColor colorL = GetRoomColor(bot.pos); // TODO: read two sensors
   int ret = 0;
   if (colorR != RoomWhite) ret += 1;
   if (colorL != RoomWhite) ret += 2;
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

long millis() {
   return GetTickCount();
}

