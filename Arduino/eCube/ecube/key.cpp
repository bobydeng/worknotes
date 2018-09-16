#include "key.h"

Key::Key() {
  initKeyPin();
  last_read = readKey();
}

void Key::on_tick_10ms() {
  int new_read = readKey();
  if ( new_read == last_read ) { //remove shake
    if ( key_state != last_read ) {
      if ( key_state == 0 && on_down ) {
        on_down();
      } else if ( key_state == 1 ) {
        if ( on_up ) {
          on_up();
        }
        if ( on_press ) {
          on_press();
        }
      }

      key_state = last_read;
      
    }
  }

  last_read = new_read;
}

void Key::set_on_press_callback(pOnPress on_press) {
  this->on_press = on_press;
}

void Key::set_on_down_callback(pOnDown on_down) {
  this->on_down = on_down;
}

void Key::set_on_up_callback(pOnUp on_up) {
  this->on_up = on_up;
}

