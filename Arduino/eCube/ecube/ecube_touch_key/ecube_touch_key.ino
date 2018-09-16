
#define PORT_TOUCH1 PIND
#define DDR_TOUCH1 DDRD
#define PORT_TOUCH2 PINB
#define DDR_TOUCH2 DDRB

#define MAX_KEY_ELAPSE  50

enum FACE {
  FRONT,
  RIGHT,
  UP,
  BACK,
  DOWN,
  LEFT
};

char face_name[] = {'F','R','U','B','D','L'};

enum DIR {
  CW,
  ACW
};

//ordered ACW starting from LEFT
enum EDGE {
  E_LEFT,
  E_BOTTOM,
  E_RIGHT,
  E_TOP
};

struct NB_KEY {
  enum EDGE edge; //shared edge
  int key;        //index of nb key
  enum DIR dir;   //on which direct of the refered key
};

struct NB_KEY nb_keys[][2] = { 
  {{E_LEFT,1,ACW}, {E_TOP, 2, CW} },     //NB keys of key 1(index 0)
  {{E_LEFT,0,CW}, {E_BOTTOM, 3, ACW} },  //NB keys of key 2(index 1)
  {{E_TOP,0,ACW}, {E_RIGHT, 3, CW} },    //NB keys of key 3(index 2)
  {{E_BOTTOM,1,CW}, {E_RIGHT, 2, ACW} }  //NB keys of key 4(index 3)
};

/**
 *  key layout
 *  
 *   ___
 *  |1|3|
 *   ---
 *  |2|4|
 *   ---
 *  
 */




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

short key_off_times[6][4];

void check_key_wipe(byte key_inputs, enum FACE face) {
  short* ot = key_off_times[face];
  face_neighber* nfs = face_neighbers[face];
  int on_keys = 0;
  for(int i=0; i < 4; i++ ){
    if(key_inputs & (1 << i)) {
      ot[i] = 0;
      on_keys++;
    } else if(ot[i] < MAX_KEY_ELAPSE) {
      ot[i]++;
    }
  }
  if(on_keys >1) {
    Serial.print("more than one keys are on, input:");
    Serial.print(key_inputs);
    Serial.print("\n");
  }

  for(int i=0; i<4; i++ ) {
    if(( key_inputs & (1<<i)) == 0) {
      continue;
    }
    for(int j=0; j<2; j++ ) {
      NB_KEY nb_k = nb_keys[i][j];
      int k_idx = nb_k.key;
      int e_idx = nb_k.edge;
      if(ot[k_idx] >= MAX_KEY_ELAPSE) {
        continue;
      }
      enum FACE nf = nfs[e_idx].face;
      Serial.write(face_name[nf]);
      if(nb_k.dir == ACW ) {
        Serial.print("'\n");
      }else {
        Serial.print("\n");
      }
      ot[k_idx] = MAX_KEY_ELAPSE; //triggered
     }
  }
}

void setup() {
  DDR_TOUCH1 = 0;
  DDR_TOUCH2 = 0;
  Serial.begin(9600);
}

void loop() {
  //front face
  byte key_inputs = PORT_TOUCH1 >> 4;
  check_key_wipe(key_inputs, FRONT);
  
  //right face keys
  key_inputs = PORT_TOUCH2 & 0x0F;
  check_key_wipe(key_inputs, RIGHT);
  
  delay(10);
}
