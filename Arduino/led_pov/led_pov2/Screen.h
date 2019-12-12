#ifndef __SCREEN__
#define __SCREEN__

//#include <arduino.h>
#include "config.h"
#include "Point.h"

#define SCR_WIDTH 112.5
#define SCR_HALF_WIDTH (SCR_WIDTH/2)
#define LED_CNT 16
#define SCR_RUNIT 2.5

#define SCR_AUNIT PI/SCR_ANGLE_RES

//15 + SCR_RUNIT/2
#define SCR_R0 16.25 

#define pin0 2
#define pin9 11
//A0 to A5
#define pin10 14
#define pin15 19

int data[2][SCR_ANGLE_RES];

class Screen {
  public:
  Screen() {
    for (int Pin = pin0; Pin <=pin9; Pin++) {
      pinMode(Pin, OUTPUT);
    }
  
    for (int Pin = pin10; Pin <=pin15; Pin++) {
      pinMode(Pin, OUTPUT);
    }
  }
  
  void drawDot(Point &c, float dotR) {
    float x = c.x - SCR_HALF_WIDTH;
    float y = c.y - SCR_HALF_WIDTH;
    float r = sqrt(x*x + y*y);
    int rm = ceil((r - SCR_R0 - dotR)/SCR_RUNIT);
    int rM = (r - SCR_R0 + dotR)/SCR_RUNIT;
    float a = atan2(y, x);
    float aExtent = asin(dotR/r);
    int am = ceil((a - aExtent)/SCR_AUNIT);
    int aM = (a + aExtent)/SCR_AUNIT;
    for(int i=rm; i<=rM; i++) {
      if(i<0 || i> LED_CNT) {
        continue;
      }
      float r1=i*SCR_RUNIT + SCR_R0;
      for(int j=am; j<=aM; j++) {
        float dis_square = r*r + r1*r1 - 2*r*r1*cos(j*SCR_AUNIT - a);
        if(dis_square < dotR*dotR) {
          writeBuff[j] |= 1<<i;
        }
      }
    }
  }
  
  void flush() {
    int* temp = activeBuff;
    activeBuff = writeBuff;
    writeBuff = temp;
  }

  void draw(int* data) {
    memcpy(data, writeBuff, sizeof(int)*SCR_ANGLE_RES);
  }

  void setScanLineData(int data, int scanLine) {
    writeBuff[scanLine] = data;
  }

  void startFrame() {
    asidx = 0;
  }

  void lineScan() {
    if(asidx < SCR_ANGLE_RES) {
      output(activeBuff[asidx]);
      asidx++;
    } else if(asidx == SCR_ANGLE_RES) {
      output(0);
    }
  }

  private:
  inline void output(int leds) {
    PORTD = leds << 2;  //0 ~ 5 -> D2 ~ D7
    PORTB = leds >> 6;  //6 ~ 9 -> B0 ~ B3
    if(leds & 0x100) { //workaround for LED8(PB2), don't know why it doesn't work in last line of code
      digitalWrite(10, HIGH);
    }else{
      digitalWrite(10, LOW);
    }
    PORTC = leds >> 9;  //10~15 -> C０~ C5

  }
  
  int* activeBuff = data[0];
  int* writeBuff = data[1];
  int asidx = SCR_ANGLE_RES;
};

#endif
