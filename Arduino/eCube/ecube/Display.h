/*******************
  Display
 ******************/

#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <Arduino.h>
#include "common.h"

#define CLK164 2  //PE4
#define DA164 3   //PE5

//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define BLK9 5    //PE3, 
#define PORT_LINE PORTA
#define DDR_LINE DDRA

#define REFRESH_PERIOD 2

class Display {
  private:
    short img_mapped[2][6][3]; //[phase][face][channel]
    int phase; //2 phases
    int channel;
    int tick; //in 100 uS
    void map_color(int phase, int channel, const enum COLOR (&face_img)[6][9]);
  public:
    Display();
    void refresh();
    void draw(const enum COLOR (&face_img)[6][9]);
};

#endif //__DISPLAY_H__
