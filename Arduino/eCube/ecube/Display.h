/*******************
  Display
 ******************/
#ifndef __DISPLAY_H__
#define __DISPLAY_H__
 
#include "common.h"
#include <Arduino.h>

#ifdef __UNO__

#define DA164 2
#define STCP595  3
//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define CLK164 12
#define BLK9 13

//PIN2, PD2, DA164
#define dataUp() (PORTD |= 0x04)  
#define dataDown() (PORTD &= 0xFB)

//PIN3, PD3, STCP595
#define regClkUp() (PORTD |= 0x08)  
#define regClkDown() (PORTD &= 0xF7)

//PIN12, PB4, CLK164
#define dataClkUp() (PORTB |= 0x10)  
#define dataClkDown() (PORTB &= 0xEF)

//PIN13, PB5, BLK9
#define blk9Up() (PORTB |= 0x20)  
#define blk9Down() (PORTB &= 0xDF)

//line selection, each face has 3 lines for red, green and blue respatively
#define PORT_LINE PORTC   //A0 - A5 of Uno
#define DDR_LINE DDRC

#else

#define CLK164 2
#define DA164 3
#define STCP595  4
//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define BLK9 5

//PIN2, PE4, CLK164
#define dataClkUp() (PORTE |= 0x10)  
#define dataClkDown() (PORTE &= 0xEF)

//PIN3, PE5, DA164
#define dataUp() (PORTE |= 0x20)  
#define dataDown() (PORTE &= 0xDF)

//PIN4, PG5, STCP595
#define regClkUp() (PORTG |= 0x20)  
#define regClkDown() (PORTG &= 0xDF)

//PIN5, PE3, BLK9
#define blk9Up() (PORTE |= 0x08)  
#define blk9Down() (PORTE &= 0xF7)

//line selection, each face has 3 lines for red, green and blue respatively
#define PORT_LINE PORTA   //PIN22 - PIN29 of Mega2560
#define DDR_LINE DDRA

#endif


#define REFRESH_PERIOD 1

class Display {
  private:
    short img_mapped[2][6][3]; //[phase][face][channel]
    int phase; //2 phases
    int face;
    int channel;
    int tick; //in 100 uS
    void map_color(int phase, int channel, const enum COLOR (&face_img)[6][9]);
  public:
    Display();
    void refresh();
    void draw(const enum COLOR (&face_img)[6][9]);
};

#endif //__DISPLAY_H__
