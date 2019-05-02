#include <iostream>

class MBot {
public:
    // coordinates
    double x = 0, y = 0;
    // current speed
    double speedX = 0, speedY = 0;

    void move(double dt) {
      x += speedX * dt;
      y += speedY * dt;
    }
};

// declare imports - expect the mBot sketch
void loop();
void setup();

int main() {
  MBot bot = MBot();
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

void setup() {

}
void loop() {

}
