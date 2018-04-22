#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <Arduino.h>
#include "common.h"

#define PORT_TOUCH1 PINB
#define DDR_TOUCH1 DDRB

#define MAX_KEY_ELAPSE  50

typedef void (*pOnWipe)(FACE face, EDGE edge, DIR dir);

struct NB_KEY {
  enum EDGE edge; //shared edge
  int key;        //index of nb key
  enum DIR dir;   //on which direct of the refered key
};

class Touch {
  private:
    short key_off_times[6][4];
    //int tick; //in 10ms
    pOnWipe on_wipe;
    void check_key_wipe(byte key_inputs, enum FACE face);
  public:
    Touch();
    void on_tick_10ms();
    void set_on_wipe_callback(pOnWipe on_wipe);
};


#endif //__TOUCH_H__

