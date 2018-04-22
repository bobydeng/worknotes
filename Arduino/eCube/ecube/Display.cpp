#include "Display.h"

/** color map, [color_index][refresh_phase][channel],
 * suppport up to 27(3*3*3) colors, to achieve the expected color, 
 * the display refresh is splitted into 2 phases, in each phase,
 * on/off of the 3 color channels are specified, so the color 
 * depth of each channel is 3(0,1,2) 
*/
int color_map[][2][3]={
  {{0, 1, 1}, {0, 1, 1}}, //RED
  {{1, 0, 1}, {1, 0, 1}}, //GREEN
  {{1, 1, 0}, {1, 1, 0}}, //BLUE
  {{0, 1, 0}, {0, 1, 0}}, //PURPLE
  {{1, 0, 0}, {1, 0, 0}}, //CYAN
  {{0, 0, 0}, {0, 0, 0}}, //WHITE
  {{0, 0, 1}, {0, 0, 1}}  //YELLOW
};


inline void write164(byte data) {
  for(int i=0; i<8; i++) {
    if(data & 0x80 ) {
      PORTE |= 0x20;  //PE5
      //digitalWrite(DA164, HIGH);
    } else {
      PORTE &= 0xDF;  //PE5
      //digitalWrite(DA164, LOW);
    }
    PORTE &= 0xEF;  //PE4
    //digitalWrite(CLK164, LOW);
    data <<= 1; //put this code here to increase puls width
    PORTE |= 0x10;  //PE4
    //digitalWrite(CLK164, HIGH);
  }
}

Display::Display() {
  DDR_LINE = 0XFF;
  PORT_LINE = 23; //0xFF //turn off all

  pinMode(CLK164, OUTPUT);
  pinMode(DA164, OUTPUT);
  pinMode(BLK9, OUTPUT);  
}

void Display::refresh() {
  tick++;
  tick %= REFRESH_PERIOD;
  if(tick) {
    return;
  }
  
  for(int face=0; face<6; face++) {
    short color = img_mapped[phase][face][channel];
    int line = face * 3 + channel;
    line = line < 6 ? line : line + 1; //workaround for a hardware defect
    PORT_LINE = 23; //0xFF //turn off all
    write164((byte) color);
    if(color & 0x100 ) {
      PORTE |= 0x08;  //PE3
      //digitalWrite(BLK9, HIGH);
    } else {
      PORTE &= 0xF7;  //PE3
      //digitalWrite(BLK9, LOW);
    }
  
    PORT_LINE = line;
  }
  channel++;
  if(channel >= 3) {
    channel = 0;
    phase++;
    phase %=2;
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


