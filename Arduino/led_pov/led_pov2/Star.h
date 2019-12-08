#ifndef __STAR__
#define __STAR__
#include <math.h>
#include "Screen.h"
#include "Point.h"

#define G 0.4

typedef void (*OnStarHome)();
typedef void (*OnStarDisappear)();

Point farPt(-1000.0, -1000.0);

class Star {
  public:
  Star(int startTime, Point & targetPos, float startY, 
    float xSpeed, OnStarHome onHome, OnStarDisappear onDisappear) {
      this->startTime = startTime;
      this->targetPos = targetPos;
      this->xSpeed = xSpeed;
      this->fallTime = sqrt(2* (targetPos.y - startY)/G);
      startPos.x = targetPos.x + xSpeed * fallTime;
      startPos.y = startY;
      float xFromC = targetPos.x - SCR_HALF_WIDTH; //offset from screen center
      float yFromC = targetPos.y - SCR_HALF_WIDTH; 
      this->r = sqrt(xFromC*xFromC + yFromC*yFromC);
      this->angle = atan2(yFromC, xFromC);
      this->explodeSpeed = rand()*5 + 3;

      this->onHome = onHome;
      this->onDisappear = onDisappear;
  }
  
  Point& fall(int time) {
    if(homed) {
      return targetPos;
    }
    if(time <= startTime) {
      return startPos;
    }
    int delT = time - startTime;
    if(delT > fallTime) {
      homed = true;
      if(onHome) {
        onHome();
      }
      return targetPos;
    }
    float x = startPos.x - delT*xSpeed;
    float y = startPos.y + 0.5*delT*delT;
    //return new Point(x,y);
    currentPos.x = x;
    currentPos.y = y;
    return currentPos;
  }

  Point& explode(int time) {
    if(disappeared) {
      return farPt;
    }
    float r = this->r + time*explodeSpeed;
    float x = r*cos(angle) + SCR_HALF_WIDTH;
    float y = r*sin(angle) + SCR_HALF_WIDTH;
    if(x<0 || x> SCR_WIDTH || y<0 || y>SCR_WIDTH) {
      disappeared = true;
      if( onDisappear) {
        onDisappear();
      }
    }
    currentPos.x = x;
    currentPos.y = y;
    return currentPos;
  }
  private:
  int startTime, fallTime;
  Point& targetPos;
  Point startPos, currentPos;
  double xSpeed, r, angle, explodeSpeed;
  boolean homed=false, disappeared=false;
  OnStarHome onHome;
  OnStarDisappear onDisappear;
};


#endif
