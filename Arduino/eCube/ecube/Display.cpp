#include "Display.h"

/** color map, [color_index][refresh_phase][channel],
 * suppport up to 27(3*3*3) colors, to achieve the expected color, 
 * the display refresh is splitted into 2 phases, in each phase,
 * on/off of the 3 color channels are specified, so the color 
 * depth of each channel is 3(0,1,2) 
*/
/*int color_map[][2][3]={
  {{0, 1, 1}, {0, 1, 1}}, //RED
  {{1, 0, 1}, {1, 0, 1}}, //GREEN
  {{1, 1, 0}, {1, 1, 0}}, //BLUE
  {{0, 1, 0}, {0, 1, 0}}, //PURPLE
  {{1, 0, 0}, {1, 0, 0}}, //CYAN
  {{0, 0, 0}, {0, 0, 0}}, //WHITE
  {{0, 0, 1}, {0, 0, 1}}  //YELLOW
};*/

int color_map[][2][3]={
  {{1, 0, 0}, {1, 0, 0}}, //RED
  {{0, 1, 0}, {0, 1, 0}}, //GREEN
  {{0, 0, 1}, {0, 0, 1}}, //BLUE
  {{1, 0, 1}, {1, 0, 1}}, //PURPLE
  //{{0, 1, 1}, {0, 1, 1}}, //CYAN
  {{1, 1, 0}, {1, 1, 0}}, //YELLOW
  {{1, 1, 1}, {1, 1, 1}}, //WHITE
  {{1, 1, 0}, {1, 1, 0}}  //YELLOW
};

inline void write164(byte data) {
  for(int i=0; i<8; i++) {
    if(data & 0x80 ) {
      dataUp();
    } else {
      dataDown();
    }
    dataClkDown();
    data <<= 1; //put this code here to increase puls width
    dataClkUp();
  }
}


Display::Display() {
  face = 0;
  tick = 0;
  channel = 0;
  phase = 0;
  
  DDR_LINE = 0X1F;
  PORT_LINE = 23; //0xFF //turn off all

  pinMode(CLK164, OUTPUT);
  pinMode(DA164, OUTPUT);
  pinMode(STCP595,OUTPUT);  
  pinMode(BLK9, OUTPUT); 
}

void Display::refresh() {
  tick++;
  tick %= REFRESH_PERIOD;
  if(tick) {
    return;
  }
  
  short color = img_mapped[phase][face][channel];
  int line = face * 3 + channel;
  if (line == 14) { //Fix hardware bug
    line = 18;
  }
  //line = line < 6 ? line : line + 1; //workaround for a hardware defect
  //PORT_LINE = 23; //0xFF //turn off all

  write164((byte) color);
  if(color & 0x100 ) {
    blk9Up();
  } else {
    blk9Down();
  }
  
  //PORT_LINE = 23; //0xFF //turn off all
  PORT_LINE = line;
  regClkUp();
  _NOP();
  regClkDown();

  face++;
  if(face >= 6) {
    face = 0;
    channel++;
    if(channel >= 3) {
      channel = 0;
      phase++;
      phase %= 2;
    }
  }
}

void Display::draw(const enum COLOR (&face_img)[6][9]) {
  for(int phase = 0; phase < 2; phase++ ) {
    for(int channel = 0; channel<3; channel++) {
      map_color(phase, channel, face_img);
    }
    /*for(int i=0;i<6;i++) {
      for(int j=0;j<3;j++){
        Serial.print(img_mapped[phase][i][j]);
        Serial.print("\t");
      }
      Serial.print("\n");
    }  */  
  }
}

void Display::map_color(int phase, int channel, const enum COLOR (&face_img)[6][9]) {
  enum COLOR pixel;
  for(int i=0;i<6;i++) {
    short face_channel = 0;
    for(int j=0; j<8; j++) {
      pixel = face_img[i][j];
      face_channel |= color_map[pixel][phase][channel] << j;
    }

    //the ninth pixel
    pixel = face_img[i][8];
    if(color_map[pixel][phase][channel]) {
      face_channel |= 0x100;
    }
    img_mapped[phase][i][channel] = face_channel;
  }
 
}


