
#include "Cube.h"
#include "Display.h"
#include "Touch.h"
#include "key.h"

Display leds;
Cube cube;
Touch touch;
Key key;

#define MODE_DEMO 0
#define MODE_PLAY 1
#define MODE_CNT 2

int mode = MODE_DEMO;

void setup() {
  Serial.begin(9600);
  cube.set_on_turned_callback(on_turned);
  touch.set_on_wipe_callback(on_wipe);
  key.set_on_press_callback(on_press);
  cube.reset();
}

void on_turned(enum COLOR (&face_img)[6][9]){
  //Serial.print("on_turned\n");
  //cube.print_faces();
  leds.draw(face_img);
}

void on_wipe(FACE face, EDGE edge, DIR dir) {
  //Serial.print("on_wipe\n");
  cube.on_wipe(face, edge, dir);
}

struct Action {
  enum FACE face;
  enum DIR dir;
};

struct Action actions[] = {{UP, CW}, {LEFT,CW}, {DOWN, ACW}, {FRONT, CW},{LEFT, ACW}, {BACK, ACW}, {UP, CW}, {BACK, ACW}, {RIGHT, CW},
                           {RIGHT, ACW},{BACK,CW}, {UP, ACW}, {BACK, CW}, {LEFT, CW},{FRONT,ACW},{DOWN,CW},{LEFT,ACW},{UP,ACW}};
//struct Action actions[] = {{FRONT, ACW}};
int act_cnt = 18;
int steps = 0;

void on_press() {
  mode +=1;
  mode %= MODE_CNT;
  if( mode == MODE_DEMO ) {
    steps = 0; 
  }

  cube.reset();
}


int tick_100us = 0;
int tick_10ms = 0;
void loop() {
  delayMicroseconds(100);
  
  leds.refresh();
  tick_100us++;
  tick_100us %= 100;
  if(tick_100us == 0) {
    key.on_tick_10ms();
    
    if(mode != MODE_DEMO ) {
      touch.on_tick_10ms();
    }
    cube.on_tick_10ms();
    tick_10ms++;
    tick_10ms %= 100;
    if(tick_10ms == 0) {
      //Serial.print(tick_10ms);
      if(mode == MODE_DEMO){
        cube.turn(actions[steps].face, actions[steps].dir);
        steps++;
        steps %=act_cnt;
      }
      //Serial.print(steps);
      //Serial.print("\n");      
    }
  }
}

