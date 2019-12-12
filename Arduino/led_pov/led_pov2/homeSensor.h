#ifndef __HOME_SENSOR__

#define __HOME_SENSOR__

//#define U18  A7
#define  U18 12

typedef void (*OnHome)(int ht);

class HomeSensor {
  public:
  HomeSensor(OnHome onHome) {
    pinMode(U18, INPUT);
    this->onHome = onHome;
  }
  void sense() {
    ht++;
    //state = digitalRead(U18);
    /*if(analogRead(U18) > 700) {
    if(PINB & 0x10) {
      state = HIGH;
    }else{
      state = LOW;
    }*/
    state = PINB & 0x10;
    if (state != lastState && state == 0){
      if(onHome) {
        onHome(ht);
        ht = 0;
      }
    }
    lastState = state;
  }
  private:
  int state = 0;
  int lastState = 0;
  uint16_t ht = 0; //home ticks
  OnHome onHome;
};

#endif
