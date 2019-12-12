#ifndef __STAR__
#define __STAR__
#include <math.h>
#include "Screen.h"
#include "Point.h"

#define G 0.4

#define HOMED 1
#define DISAPPEARED 2

class Star {
public:
  void init(uint8_t targetX, uint8_t targetY) {
      this->targetX = targetX;
      this->targetY = targetY;

      init();
  }

  void init() {
    flags = 0;
    
    startTime = rand()*200;
    startY = rand()*(-11) - 28;
    xSpeed = random()*2.8 + 1.7;
    fallTime = sqrt(2* (targetY - startY)/G);
    startX = targetX + xSpeed * fallTime;
  }

  //return true if homed
  boolean fall(Point& currentPos) {
    if(homed()) {
      currentPos.x = targetX;
      currentPos.y = targetY;
      return true;
    }
    if(time <= startTime) {
      currentPos.x = startX;
      currentPos.y = startY;
    }
    int delT = time - startTime;
    if(delT > fallTime) {
      setHomed();
      currentPos.x = targetX;
      currentPos.y = targetY;
      return true;
    }
    float x = startX - delT*xSpeed;
    float y = startY + 0.5*delT*delT;
    currentPos.x = x;
    currentPos.y = y;
    return false;
  }

   //return true if disappeared
   boolean explode(Point& currentPos) {
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
    
    r = r + time*xSpeed; //resuse xSpeed as explode speed, to reduce memory usage
    float x = r*cos(angle) + SCR_HALF_WIDTH;
    float y = r*sin(angle) + SCR_HALF_WIDTH;
    if(x<0 || x> SCR_WIDTH || y<0 || y>SCR_WIDTH) {
      setDisappeared();
      currentPos.x = -1000.0;
      currentPos.y = -1000.0;
      return true;
    }
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
  uint8_t startTime, fallTime;
  uint8_t targetX, targetY, startX, startY;
  uint8_t xSpeed;
  uint8_t flags;
  static int time;

  inline boolean homed() {
    return flags & HOMED;
  }

  inline boolean disappeared() {
    return flags & DISAPPEARED;
  }

  inline void setHomed() {
    flags |= HOMED;
  }

  inline void setDisappeared() {
    flags |= DISAPPEARED;
  }
};

int Star::time = 0;

#endif
