#ifndef __COMMON_H__
#define __COMMON_H__

enum FACE {
  FRONT,
  RIGHT,
  UP,
  BACK,
  DOWN,
  LEFT
};

//ordered ACW
enum EDGE {
  E_LEFT,
  E_BOTTOM,
  E_RIGHT,
  E_TOP
};

enum DIR {
  CW,  //clockwise
  ACW  //anti clockwise
};

enum COLOR {
  RED,
  GREEN,
  BLUE,
  PURPLE,
  CYAN,
  WHITE,
  YELLOW
};

char get_face_name(FACE face);

#endif // __COMMON_H__


