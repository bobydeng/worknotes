#ifndef __STAR__
#define __STAR__
#include <math.h>
#include "Screen.h"
#include "Point.h"

// 1 cs = 0.01s = 10ms
// G in mm/cs^2
//#define G 0.4
//#define G 0.024
#define G 0.05


#define HOMED 1
#define DISAPPEARED 2

#define xSpeedRate 10.0


class Star {
public:
  void init(uint8 targetX, uint8 targetY) {
      this->targetX = targetX;
      this->targetY = targetY;

      init();
  }

  void init() {
    flags = 0;
    
    startTime = rand()%150;
    startY = -rand()%5 - 5;
    //xSpeed = rand()%3 + 1.7;
    xSpeed = rand()%60 + 30;  // 60~90 mm/s = 0.6~0.9 mm/cs, store in uint8 to reduce memeory usage
    fallTime = sqrt(2* (targetY - startY)/G);
    startX = targetX + ((int)xSpeed) * fallTime/xSpeedRate;
  }

  //return true if homed
  bool fall(Point& currentPos) {
    if(homed()) {
      currentPos.x = targetX;
      currentPos.y = targetY;
      return true;
    }
    if(time <= startTime) {
      currentPos.x = startX;
      currentPos.y = startY;
      return false;
    }
    int delT = time - startTime;
    if(delT > fallTime) {
      setHomed();
      currentPos.x = targetX;
      currentPos.y = targetY;
      return true;
    }
    float x = startX - delT*(xSpeed/xSpeedRate);
    float y = startY + 0.5*delT*delT*G;
    currentPos.x = x;
    currentPos.y = y;
    return false;
  }

   //return true if disappeared
   bool explode(Point& currentPos) {
    if(disappeared()) {
      currentPos.x = -1000.0;
      currentPos.y = -1000.0;
      return true;
    }

    //calculate online to reduce memory usage
    float xFromC = targetX - SCR_HALF_WIDTH; //offset from screen center
    float yFromC = targetY - SCR_HALF_WIDTH; 
    float r = sqrt(xFromC*xFromC + yFromC*yFromC);
    float angle = atan2(yFromC, xFromC);

    //resuse xSpeed as explode speed, to reduce memory usage
    r = r + time*(xSpeed/xSpeedRate);
    if(r> SCR_HALF_WIDTH) {
      setDisappeared();
      currentPos.x = -1000.0;
      currentPos.y = -1000.0;
      return true;
    }

    float x = r*cos(angle) + SCR_HALF_WIDTH;
    float y = r*sin(angle) + SCR_HALF_WIDTH;

    currentPos.x = x;
    currentPos.y = y;
    return false;
  }

  static void tick() {
    Star::time++;
  }

  static void resetTime() {
    Star::time = 0;
  }

private:
  uint8 startTime, fallTime;
  uint8 targetX, targetY, startX;
  char startY;
  uint8 xSpeed;
  uint8 flags;
  static int time;

  inline bool homed() {
    return flags & HOMED;
  }

  inline bool disappeared() {
    return flags & DISAPPEARED;
  }

  inline void setHomed() {
    flags |= HOMED;
  }

  inline void setDisappeared() {
    flags |= DISAPPEARED;
  }
};


#endif
