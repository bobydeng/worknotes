#ifndef __POINT__
#define __POINT__
class Point {
  public:
  Point(){}
  Point(float x, float y) {
    this->x = x;
    this->y = y;
  }
  float x, y;
};

/*
class PolarPoint {
  public:
  PolarPoint(){}
  PolarPoint(float r, float a) {
    this->r = r;
    this->a = a;
  }

  float r, a;
};
*/

#endif
