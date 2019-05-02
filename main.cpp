#include <iostream>


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
      // TODO: heading

      x += speed * dt;
      y += 0;
    }
};


// declare imports - expect the mBot sketch
void loop();
void setup();

int main() {
// forward declare motors
  extern MeDCMotor motor_9;
  extern MeDCMotor motor_10;
  MBot bot = MBot(&motor_9, &motor_10);
  printf("Emulator started - coordinates %.3f,%.3f\n", bot.x, bot.y);
  double dt = 0.1;
  setup();
  printf("Setup done");
  for (int i = 0; i < 1000; i++) {
    loop();
    bot.move(dt);
  }
  printf("Final bot coordinates %.3f,%.3f\n", bot.x, bot.y);
  return 0;
}

MeDCMotor motor_9(9);
MeDCMotor motor_10(10);

void setup() {
  motor_9.run((9)==M1?-(50):(50));
  motor_10.run((10)==M1?-(50):(50));
}
void loop() {

}
