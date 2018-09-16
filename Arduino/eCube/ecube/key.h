#ifndef __KEY_H__
#define __KEY_H__

#include <Arduino.h>
#include "common.h"

#define initKeyPin() (DDRC &= 0xDF) 
#define readKey() ((PINC & 0x20) >> 5)

typedef void (*pOnPress)();
typedef void (*pOnDown)();
typedef void (*pOnUp)();

class Key {
  private:
    int key_state=-1;
    int last_read=-1;
    pOnPress on_press;
    pOnDown on_down;
    pOnUp on_up;
  public:
    Key();
    void on_tick_10ms();
    void set_on_press_callback(pOnPress on_press);
    void set_on_down_callback(pOnDown on_down);
    void set_on_up_callback(pOnUp on_up);
};

#endif //__KEY_H__
