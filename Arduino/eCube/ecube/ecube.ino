
#define CLK164 2  //PE4
#define DA164 3   //PE5

//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define BLK9 5    //PE3, 
#define PORT_LINE PORTA
#define DDR_LINE DDRA

enum FACE {
  FRONT,
  RIGHT,
  UP,
  BACK,
  DOWN,
  LEFT
};

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

int color_map[][3]={
  {0, 1, 1}, //RED
  {1, 0, 1}, //GREEN
  {1, 1, 0}, //BLUE
  {0, 1, 0}, //PURPLE
  {1, 0, 0}, //CYAN
  {0, 0, 0}, //WHITE
  {0, 0, 1}  //YELLOW
};

enum COLOR img[6][9];

short img_compressed[6][3];

void setup() {
  DDR_LINE = 0XFF;
  PORT_LINE = 23; //0xFF //turn off all

  pinMode(CLK164, OUTPUT);
  pinMode(DA164, OUTPUT);
  pinMode(BLK9, OUTPUT);

  Serial.begin(9600);
  reset_faces();

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
void loop() {
  turn(actions[steps].face, actions[steps].dir);
  //printface();
  compress_img();
  for(int t=0;t<200;t++) {
    for(int i=0;i<3;i++) {
      write_channel(i);
    }
  }
  steps +=1;
  steps %=act_cnt;
  if(steps == 0){
    for(int t=0;t<2000;t++) {
      for(int i=0;i<3;i++) {
        write_channel(i);
      }
    }
  }
  //Serial.print(steps);
  //Serial.print("\n");
}

void reset_faces() {
  for(int i=0; i< 6; i++) {
    for( int j=0; j<9; j++) {
      img[i][j] = i;
    }
    /*for( int j=0; j<3; j++) {
      img[i][j] = 0;
    }
    for( int j=3; j<6; j++) {
      img[i][j] = 1;
    }
    for( int j=6; j<9; j++) {
      img[i][j] = 2;
    }*/
  }

  printface();

  compress_img();
  
  for(int i=0;i<6;i++) {
    for(int j=0;j<3;j++){
      Serial.print(img_compressed[i][j]);
      Serial.print("\t");
    }
    Serial.print("\n");
  }
  
  
}

void printface(){
  for(int i=0;i<6;i++) {
    for(int j=0;j<9;j++){
      Serial.print(img[i][j]);
      Serial.print("\t");
    }
    Serial.print("\n");
  }
   Serial.print("-------------------------\n");
}

void compress_img(int channel) {
  enum COLOR pixel;
  for(int i=0;i<6;i++) {
    short face_channel = 0;
    for(int j=0; j<8; j++) {
      pixel = img[i][j];
      face_channel |= color_map[pixel][channel] << j;
    }

    //the ninth pixel
    pixel = img[i][8];
    if(color_map[pixel][channel]) {
      face_channel |= 0x100;
    }
    img_compressed[i][channel] = face_channel;
  }
}

void compress_img() {
  for(int i=0; i<3; i++) {
    compress_img(i);
  }
}

void write_channel(byte channel) {
  for(int i=0;i<6;i++) {
    write_cube_face(i, channel, img_compressed[i][channel]);
  }
}

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

void write_cube_face(int face, int channel, short color) {
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
  delayMicroseconds(200);
}

/**
 *     face layout
 *         -------
 *        |       |
 *        |  TOP  |
 *        |       |
 *  -----------------------------
 * |      |       |       |      |
 * | LEFT | FRONT | RIGHT | BACK |
 * |      |       |       |      |
 *  -----------------------------
 *        |       |
 *        |BOTTOM |
 *        |       |
 *         -------
 *         
 *  pixel index, same for all faces in orientation specified above
 *   _____
 *  |1|4|7|
 *   -----
 *  |2|5|8|
 *  ------
 *  |3|6|9|  
 *   -----
 *   
 *   
 */

// index starts with zero, edge defined with pixel index in anti-clockwise direction
int edge_pixel_idxs[][3] = { 
  { 0, 1, 2}, //left edge
  { 2, 5, 8}, //bottom edge
  { 8, 7, 6}, //right edge
  { 6, 3, 0}, //top edge
};

struct face_neighber {
  enum FACE face;
  enum EDGE edge;
};

face_neighber face_neighbers[6][4] = { 
  //NB of FRONT face
  {{LEFT, E_RIGHT}, 
  {DOWN, E_TOP}, 
  {RIGHT, E_LEFT}, 
  {UP, E_BOTTOM} }, 
  //NB of RIGHT face
  {{FRONT, E_RIGHT}, 
  {DOWN, E_RIGHT}, 
  {BACK, E_LEFT}, 
  {UP, E_RIGHT} }, 
  //NB of UP face
  {{LEFT, E_TOP}, 
  {FRONT, E_TOP}, 
  {RIGHT, E_TOP}, 
  {BACK, E_TOP} }, 
  //NB of BACK face
  {{RIGHT, E_RIGHT}, 
  {DOWN, E_BOTTOM}, 
  {LEFT, E_LEFT}, 
  {UP, E_TOP} },
  //NB of DOWN face
  {{LEFT, E_BOTTOM}, 
  {BACK, E_BOTTOM}, 
  {RIGHT, E_BOTTOM}, 
  {FRONT, E_BOTTOM} },
  //NB of LEFT face
  {{BACK, E_RIGHT}, 
  {DOWN, E_LEFT}, 
  {FRONT, E_LEFT}, 
  {UP, E_LEFT} }, 
};

void turn(enum FACE face, enum DIR dir) {
  enum COLOR temp_nb_edge[3], temp_edge[3];
  int to_idx,from_idx, inc;
  if(dir == ACW) {
    to_idx = 3;
    inc = -1;
  } else {
    to_idx = 0;
    inc = 1;
  }

  //copy edge of the turnning face to temp buff
  copy(face, to_idx, temp_edge);
  face_neighber from_nb, to_nb = face_neighbers[face][to_idx];
  //copy first neighber to temp array
  copy(to_nb.face, to_nb.edge, temp_nb_edge);
  for(int i=0;i<3; i++ ){
    from_idx = to_idx + inc;
    //copy edge of the turnning face
    copy(face, from_idx, to_idx);
    from_nb = face_neighbers[face][from_idx];
    //copy edge of the neighber faces
    copy(from_nb.face, from_nb.edge, to_nb.face, to_nb.edge);
    to_idx = from_idx;
    to_nb = from_nb;
  }
  
  //copy temp buf to the edge of the turnning face
  copy(temp_edge, face, to_idx);
  //copy the temp array to the last neighber
  copy(temp_nb_edge, to_nb.face, to_nb.edge);
}

void copy(int from_face, int from_edge, enum COLOR* edge_buf) {
  for(int i=0;i<3;i++) {
     edge_buf[i] = img[from_face][edge_pixel_idxs[from_edge][i]];
  }
}

void copy(enum COLOR* edge_buf, int to_face, int to_edge) {
  for(int i=0;i<3;i++) {
     img[to_face][edge_pixel_idxs[to_edge][i]] = edge_buf[i];
  }
}

void copy(int from_face, int from_edge, int to_face, int to_edge) {
  for(int i=0;i<3;i++) {
     img[to_face][edge_pixel_idxs[to_edge][i]] = img[from_face][edge_pixel_idxs[from_edge][i]];
  }
}

void copy(int face, int from_edge, int to_edge) {
  enum COLOR temp_edge[3];
  copy(face, from_edge, temp_edge);
  copy(temp_edge, face, to_edge);
}

