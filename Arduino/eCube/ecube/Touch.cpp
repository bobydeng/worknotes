#include "Touch.h"

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

struct NB_KEY nb_keys[][2] = { 
  {{E_LEFT,1,ACW}, {E_TOP, 2, CW} },     //NB keys of key 1(index 0)
  {{E_LEFT,0,CW}, {E_BOTTOM, 3, ACW} },  //NB keys of key 2(index 1)
  {{E_TOP,0,ACW}, {E_RIGHT, 3, CW} },    //NB keys of key 3(index 2)
  {{E_BOTTOM,1,CW}, {E_RIGHT, 2, ACW} }  //NB keys of key 4(index 3)
};

Touch::Touch() {
  initKeysF();
  initKeysR();
}

void Touch::set_on_wipe_callback(pOnWipe on_wipe) {
  this->on_wipe = on_wipe;
}

void Touch::check_key_wipe(byte key_inputs, enum FACE face) {
  short* ot = key_off_times[face];
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
      if(ot[k_idx] >= MAX_KEY_ELAPSE) {
        continue;
      }
      if(on_wipe) {
        on_wipe(face, nb_k.edge, nb_k.dir);
      }
      ot[k_idx] = MAX_KEY_ELAPSE; //triggered
    }
  }
}

void Touch::on_tick_10ms() {
  //front face keys
  byte key_inputs = readKeysF();
  check_key_wipe(key_inputs, FRONT);

  //right face
  key_inputs = readKeysR();

  //Fix hardware defect
  /*byte tmp = key_inputs;
  key_inputs &= 0xF3;
  if(tmp & 0x4) {
    key_inputs |= 0x08;
  }
  if(tmp & 0x8) {
    key_inputs |= 0x04;
  }*/
  
  check_key_wipe(key_inputs, RIGHT);
}


