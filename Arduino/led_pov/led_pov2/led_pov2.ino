#include "Star.h"
#include "homeSensor.h"
#include "Screen.h"
#include "player.h"

//angle scan period
#define ASP 100

#ifdef __TEST__
int asidx =0 ;
int toggle = 0;
#endif

Screen screen;

#ifdef USING_TIMER
inline void onHome(int ht/*home_period*/) {
  //home_frequence = tick_requence/ht --> ht = tick_requence/home_frequence
  if ( ht > 100 && ht < 10000){ //between 1 ~ 100 hz, only accept freqence in then between
    //angle angle scanning frequence:
    //    asf = home_frequence * SCR_ANGLE_RES 
    //        = SCR_ANGLE_RES * tick_requence/home_period_t
    //        = SCR_ANGLE_RES * 10000/home_period_t
    //OCR1A = 16*10^6/asf - 1
    //      = home_period_t * 16*10^6/(SCR_ANGLE_RES * 10000) - 1
    //      = home_period_t * 1600/SCR_ANGLE_RES - 1
    OCR1A =  ((uint32_t)ht) * 1600/SCR_ANGLE_RES - 1;
    TCNT1  = 0;

  }
  screen.startFrame();
}

#else
inline void onHome(int ht) {
  screen.startFrame();
}
#endif

HomeSensor homeSensor(onHome);
Player player(screen);

//set timer0 interrupt at 100hz, for updating of image to be displayed
void init_timer0() {
  TCCR0A = 0;// set entire TCCR2A register to 0
  TCCR0B = 0;// same for TCCR2B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 100hz increments
  OCR0A = 155;// = (16*10^6) / (100*1024) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS02 and CS00 bits for 1024 prescaler
  TCCR0B |= (1 << CS02) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
}

//set timer1 interrupt at 60*ANGLE_LINES Hz for angle scanning of display fresh, 
//frequence will be updated according to the turning frequence
void init_timer1() {
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 60*ANGLE_LINES hz increments
  OCR1A = 740;// = (16*10^6) / (21600) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  //TCCR1B |= (1 << CS12) | (1 << CS10);  
  // Set CS10 bits for 1 prescaler
  TCCR1B |= (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

//set timer2 interrupt at 10kHz , for home checking 
void init_timer2() {
  TCCR2A = 0;// set entire TCCR2A register to 0
  TCCR2B = 0;// same for TCCR2B
  TCNT2  = 0;//initialize counter value to 0
  // set compare match register for 10khz increments
  OCR2A = 199;// = (16*10^6) / (10000*8) - 1 (must be <256)
  // turn on CTC mode
  TCCR2A |= (1 << WGM21);
  // Set CS21 bit for 8 prescaler
  TCCR2B |= (1 << CS21);   
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
}

ISR(TIMER0_COMPA_vect){
#ifdef __TEST__  
  digitalWrite(pin0, HIGH);
#else
  player.play();
#endif
}

//int toggle = 0; 
ISR(TIMER1_COMPA_vect){
#ifdef __TEST__  
  asidx++;
  asidx%=10000;
  if(asidx==0) {
    if(toggle) {
      digitalWrite(pin0+1, HIGH);
      toggle = 0;
    } else {
      digitalWrite(pin0+1, LOW);
      toggle = 1;
    }
  }
#else
  screen.lineScan();
#endif
}

ISR(TIMER2_COMPA_vect){
#ifdef __TEST__
  //digitalWrite(pin0+2, state);
  asidx++;
  asidx%=10000;
  if(asidx==0) {
    if(toggle) {
      digitalWrite(pin0+2, HIGH);
      toggle = 0;
    } else {
      digitalWrite(pin0+2, LOW);
      toggle = 1;
    }
  }
#else
  homeSensor.sense();
#endif
}

void setup() {
#ifdef USING_TIMER
  cli();//stop interrupts
  init_timer0();
  init_timer1();
  init_timer2();
  sei();//allow interrupt
#endif
}

void loop() {
#ifndef USING_TIMER 
  homeSensor.sense();
  screen.lineScan();
  //player.play();
  delayMicroseconds(ASP);
#endif
}



