#ifndef __PLAYER__

#define __PLAYER__

#include "Screen.h"

class Player {
  public:
  Player(Screen& screen):screen(screen) {
  }
  void tick() {
    iut++;
    iut%=500;
    if(iut == 0) {
      screen.flush(); //switch image buffer;
    }
  }
  private:
  int iut = 0; //image update ticks
  Screen& screen;
};
#endif
