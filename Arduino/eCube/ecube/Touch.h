#ifndef __TOUCH_H__
#define __TOUCH_H__

#include <Arduino.h>
#include "common.h"

#ifdef __UNO__

#define PORT_TOUCHF PIND   //PD4 - PD7 : PIN4 - PIN7 of UNO
#define DDR_TOUCHF DDRD

#define PORT_TOUCHR PINB   //PB0 - PB3 : PIN8 - PIN11 of UNO
#define DDR_TOUCHR DDRB

#define initKeysF() (DDR_TOUCHF &= 0x0F)
#define readKeysF() (PORT_TOUCHF >> 4)

#define initKeysR() (DDR_TOUCHR &= 0xF0)
#define readKeysR() (PORT_TOUCHR & 0x0F)

#else

#define PORT_TOUCH1 PINB   //PB0 - PB3 : PIN53 - PIN50 of Mega2560 
#define DDR_TOUCH1 DDRB

#define PORT_TOUCHR PINB   //PB4 - PB7 : PIN10 - PIN13 of Mega2560
#define DDR_TOUCHR DDRB

#define initKeysF() (DDR_TOUCHR &= 0xF0)
#define readKeysF() (PORT_TOUCHF & 0x0F)

#define initKeysR() (DDR_TOUCHF &= 0x0F)
#define readKeysR() (PORT_TOUCHF >> 4)

#endif


#define MAX_KEY_ELAPSE  25

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

