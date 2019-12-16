#ifndef __PLAYER__

#define __PLAYER__

#include "Screen.h"
#include "Star.h"

const int imgs[][SCR_ANGLE_RES] PROGMEM = {
{40960,40960,40960,40960,40960,40968,32776,32776,36864,36880,
36880,38944,34880,36800,34688,32768,32768,32896,33216,33728,
33536,34304,1536,1024,1024,1024,1024,1028,1030,33798,
1028,9732,42508,42760,41880,13296,45552,12512,6144,38912,
7168,3072,35840,3584,3840,35584,7040,6336,6368,6240,
6624,7040,7040,7936,7680,3072,6144,6144,14336,12288,
28704,24688,8432,8640,896,1792,1920,3968,7296,7296,
6340,6220,4220,4216,48,48,96,96,192,192,
128,384,384,0,0,0,0,0,1920,1984,
3168,2096,2064,4096,4104,4104,4096,4096,4100,4100,
4100,4096,4096,4104,4104,4104,6160,2064,2080,3168,
1984,896,0,0,32768,0,0,0,0,0,
0,512,768,768,384,384,384,192,224,480,
2032,8184,8156,40140,6208,12352,41024,32864,96,32800,
32,32816,32816,40960,45056,48640,16320,49120,45024,42528,
42528,42528,42528,42528,41504,41504,41504,41504,41504,41472,
41472,40960,40976,45112,45176,41192,33224,33668,33540,34564,
34308,35846,35846,35844,35844,33804,34316,34328,33592,33776,
33248,32960,32768,32768,32768,32768,32768,32768,32768,33536,
34688,36032,34912,36896,36880,36880,36872,32776,40968,40960}
,
{36352,36352,33792,32768,32768,32768,32832,33008,33016,33272,
33276,33276,33276,33276,33278,33278,33278,33278,33276,33020,
33020,32888,112,0,0,0,0,0,0,32768,
0,6144,38912,39936,39936,7168,39936,7168,6144,38912,
0,96,32864,112,112,32864,0,0,0,0,
0,0,0,0,0,256,896,1984,1984,1984,
2016,2016,2016,2016,1984,1984,960,896,0,0,
0,0,0,0,0,0,0,96,240,240,
240,240,224,96,0,0,0,0,0,0,
128,480,496,1008,1016,1016,1016,1016,1016,1016,
1016,1016,1016,1008,496,480,192,0,4096,6144,
6144,6144,6144,6144,38912,0,48,56,120,120,
120,120,112,32,0,0,0,768,1792,1792,
1920,1920,1792,33536,768,0,32768,32768,0,32768,
0,32768,32768,32768,32768,33152,960,34784,34800,36848,
36856,36856,36856,36856,36856,36860,36860,36860,36860,36856,
36856,36856,36848,34800,34800,33760,33728,32768,32768,32768,
32768,32768,32880,32880,33016,33016,33016,33016,33008,33008,
34400,34304,34560,34560,34560,34304,34312,32796,32830,32830,
32830,32830,34366,36382,36380,36608,36608,36608,36608,36608}
};

const uint8_t SHData[][2] PROGMEM = {
{56, 89},
{53, 87},
{50, 86},
{47, 84},
{44, 82},
{41, 80},
{39, 77},
{36, 75},
{34, 72},
{32, 69},
{31, 66},
{29, 63},
{28, 59},
{27, 56},
{26, 52},
{26, 49},
{26, 45},
{27, 42},
{28, 39},
{29, 35},
{31, 32},
{33, 30},
{36, 28},
{40, 27},
{43, 26},
{47, 27},
{50, 28},
{53, 30},
{55, 33},
{56, 36},
{58, 33},
{60, 30},
{63, 28},
{66, 27},
{69, 26},
{73, 27},
{76, 28},
{79, 30},
{82, 32},
{84, 35},
{85, 39},
{86, 42},
{86, 45},
{86, 49},
{86, 52},
{86, 56},
{85, 59},
{83, 63},
{82, 66},
{80, 69},
{78, 72},
{76, 75},
{74, 77},
{71, 80},
{69, 82},
{66, 84},
{63, 86},
{60, 87}
};

const uint8_t BHData[][2] PROGMEM = {
{56, 96},
{52, 94},
{48, 92},
{45, 90},
{41, 88},
{38, 85},
{35, 82},
{32, 79},
{29, 75},
{27, 72},
{25, 68},
{23, 64},
{22, 60},
{20, 56},
{20, 52},
{19, 47},
{19, 43},
{20, 39},
{21, 35},
{23, 31},
{25, 27},
{28, 24},
{32, 22},
{36, 20},
{40, 20},
{45, 20},
{49, 22},
{52, 25},
{54, 28},
{56, 32},
{58, 28},
{61, 25},
{64, 22},
{68, 20},
{72, 20},
{76, 20},
{80, 22},
{84, 24},
{87, 27},
{90, 31},
{91, 35},
{92, 39},
{93, 43},
{93, 47},
{93, 52},
{92, 56},
{91, 60},
{89, 64},
{88, 68},
{86, 72},
{83, 75},
{81, 79},
{78, 82},
{75, 85},
{71, 88},
{68, 90},
{64, 92},
{60, 94}
};

#define STAR_NUM (sizeof(SHData)/2)
#define DOT_R 2.0


class Scene {
public:
  virtual void prepare() = 0;
  virtual bool play() = 0; //return true if done
};

class Scene0: public Scene {
public:
  Scene0(Screen & screen):screen(screen) {}
  void prepare(){
    iut = 0;
    mst = 0;
  }
  
  bool play() {
    iut%=300;
    if(iut == 0) {
      if(mst > 1) {
        return true;
      }
      screen.draw_P(imgs[mst]);
      screen.flush(); //switch image buffer;
      mst++;
    }
    iut++;
    return false;
  }
private:
  Screen & screen;
  int iut; //image update ticks
  uint8 mst; //micro steps
};

class Scene1: public Scene{
public:
  Scene1(Screen& screen, Star* stars):screen(screen),stars(stars) {
  }
  void prepare() {
    Star::resetTime();
    done = false; //reset for next round
    iut = 0;
  }
  bool play() {
    if(iut == 0) {
      if(done) {
        return true;
      }
      Star::tick();
      int doneCnt = 0;
      Point cp;
      screen.clear();
      for(int i=0; i< STAR_NUM; i++ ) {
        bool oneDone = starAct(stars[i], cp);
        screen.drawDot(cp.x, cp.y, DOT_R);
        if(oneDone) {
           doneCnt++;
        }
      }
      screen.flush();
#ifdef __DEBUG__
      screen.dumpData();
#endif
      done = doneCnt == STAR_NUM; //just flag it, not really done yet, one more frame to show
    }
    iut++;
    iut%=1;//10;
    return false;
  }

protected:
  virtual bool starAct(Star& star, Point& cp) {
    return star.fall(cp);
  }
private:
  Screen& screen;
  Star* stars;
  bool done=false;
  int iut;
};

class Scene2: public Scene {
public:
  Scene2(Screen& screen):screen(screen) {}
  void prepare() {
    screen.clear();
    for(int i=0; i< STAR_NUM; i++ ) {
      int point = pgm_read_word(BHData[i]);
      screen.drawDot((uint8)point, point>>8, DOT_R);
    }
       
    iut=0;
    reliefPeriod = 60;
    isRelief = true;
  }
  bool play() {
    iut++;

    int period = isRelief? reliefPeriod : reliefPeriod/2;
    if(iut >= period) {
      screen.flush(); //switch buffer
      isRelief != isRelief;
      reliefPeriod--;
      iut = 0;
    }
    
    if(isRelief && reliefPeriod < 40) {
      return true;
    }
    return false;
  }
private:
  Screen & screen;
  int iut; //image update ticks
  int reliefPeriod;
  bool isRelief; //relief or shrink
};

class Scene3: public Scene1{
public:
  Scene3(Screen& screen, Star* stars):Scene1(screen,stars) {
  }
protected:
  bool starAct(Star& star, Point& cp) {
    return star.explode(cp);
  }
};

#define SCENE_NUM 4

class Player {
 public:
  Player(Screen& screen) {
    for(int i=0; i< STAR_NUM; i++ ) {
      int point = pgm_read_word(SHData[i]);
      //heartStars[i].init(SHData[i][0],SHData[i][1]);
      heartStars[i].init(point, point>>8);
    }
    
    scenes[0] =  new Scene0(screen);
    scenes[1] =  new Scene1(screen, heartStars);
    scenes[2] =  new Scene2(screen);
    scenes[3] =  new Scene3(screen, heartStars);
    //scenes[0] =  scenes[1];
    scenes[0]->prepare();
  }

  ~Player() {
    for(int i=0; i<SCENE_NUM; i++ ) {
      delete scenes[i];
    }
  }
  
  void play() {
    bool done=scenes[steps]->play();
    if(done) {
      steps++;
      steps %= SCENE_NUM;
      if(steps == 0) { //start new round, re-initialize stars
        initStars();
      }
      scenes[steps]->prepare();
    }
  }

private:
  Scene* scenes[SCENE_NUM];
  Star heartStars[STAR_NUM];
  uint8 steps=0;
  
  void initStars() {
    for(int i=0; i< STAR_NUM; i++ ) {
      heartStars[i].init();
    }
  }
};
#endif
