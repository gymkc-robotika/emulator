#ifndef MEM_CORE_H
#define MEM_CORE_H


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

class MeRGBLed {
   public:
   int myNumber;
   
   explicit MeRGBLed(int number, int dummy) {
      myNumber = number;
   }
   
   void setColor(int leftRight, int r, int g, int b) {
   }
   void show() {
   }
};

#define A7 0xa000 + 7

static void pinMode(int pin, int mode) {
}
static int analogRead(int pin) {
   return 0;
}
static long millis() {
   return GetTickCount();
}

#endif

