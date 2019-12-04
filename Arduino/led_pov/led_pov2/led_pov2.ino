//splits of one turn
#define ANGLE_LINES  360
//angle scan period
#define ASP 100
#define USING_TIMER __using_timer__

//int U18 = A7;
int U18 = 12;
int state = 0;
int lastState = 0;
int pin0 = 2;
int pin9 = 11;
int pin10 = 14; //A0;
int pin15 = 19; //A5;
boolean startFrame = false;

uint16_t  ht = 0; //home ticks
uint16_t  lht = 0; //last home tick
int ab = 0; //active buffer
int asidx = ANGLE_LINES; //angle scanning index
int iut = 0; //image updage ticks

const int data[][ANGLE_LINES] /*PROGMEM*/ = {
{40960,40960,40960,40960,40960,40960,40960,40960,40968,40968,
40968,32776,32776,36872,36864,36880,36880,36880,36880,36896,
38944,34912,34880,36800,34688,34688,34560,32768,32768,32768,
32768,33152,33216,33216,33728,33536,33536,34304,34304,34304,
1024,33792,1024,1024,33792,1024,33792,1024,1028,33796,
1030,33798,33796,33796,1028,33796,9732,42500,9740,9740,
42760,41880,46008,13296,46064,45552,12768,12480,38912,6144,
38912,38912,7168,3072,3072,35840,36352,3584,3584,3840,
3840,35584,39808,6528,6592,6336,6368,6240,6240,6368,
6624,6592,7040,7040,8064,7936,7936,7680,7168,3072,
7168,6144,6144,6144,14336,12288,12288,12288,28704,24608,
24688,24816,8416,8672,9152,896,896,1792,1792,3968,
3968,3968,3200,7296,7296,6340,6340,6350,6220,4188,
4216,4216,4152,48,48,48,96,96,96,96,
192,192,192,192,128,384,384,384,384,384,
0,0,0,0,0,0,0,256,896,1984,
1984,3168,3168,2080,2096,2064,2064,4096,4104,4104,
4104,4104,4104,4096,4096,4100,4100,4100,4100,4100,
4100,4100,4096,4096,4096,4104,4104,4104,4104,4104,
4096,6160,2064,2064,2080,3104,3168,1728,1984,896,
768,0,0,0,0,32768,0,0,32768,0,
0,0,0,0,0,0,0,512,512,768,
768,768,768,768,384,384,384,448,192,192,
192,224,224,1008,2032,36848,8184,8152,7900,7756,
40012,6208,36928,12352,45120,41024,41024,32864,32864,96,
32800,32800,32,32800,32816,32816,32816,8208,40960,45056,
47104,48640,48896,16320,49120,49120,49120,42976,42528,42528,
42528,42528,42528,42528,42528,42528,42528,41504,41504,41504,
41504,41504,41504,41520,41520,41472,41472,41472,41472,40960,
40960,40976,40976,41016,45176,45176,41192,41192,33224,33224,
33164,33668,33540,33540,34564,34308,34308,36356,35846,35846,
35844,35844,35844,35844,35852,33804,34316,34316,34328,34328,
33592,33584,33776,33776,33248,32992,32960,32768,32768,32768,
32768,32768,32768,32768,32768,32768,32768,32768,32768,32768,
33536,34688,34688,36800,36032,34912,34848,36896,36880,36880,
36880,36880,36864,36872,32776,40968,40968,40968,40968,40960}
,
{36352,36352,36352,34304,33792,32768,32768,32768,32768,32768,
32768,32832,32864,33008,33008,33016,33272,33272,33276,33276,
33276,33276,33276,33276,33276,33278,33278,33278,33278,33278,
33278,33278,33276,33276,33020,33020,33020,33020,32888,32888,
48,32768,0,0,32768,0,32768,0,0,32768,
0,32768,32768,32768,0,38912,6144,38912,7168,6144,
39936,39936,39936,7168,39936,39936,7168,7168,38912,6144,
38912,32768,0,32,96,32864,32880,112,112,112,
112,32864,32864,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,256,
896,896,960,1984,1984,1984,1984,1984,2016,2016,
2016,2016,2016,1984,1984,1984,1984,1984,960,960,
960,896,256,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,32,96,112,
112,240,240,240,240,240,240,240,96,96,
96,0,0,0,0,0,0,0,0,0,
0,0,128,480,480,496,496,1008,1008,1016,
1016,1016,1016,1016,1016,1016,1016,1016,1016,1016,
1016,1016,1016,1016,1016,1008,1008,496,496,480,
480,64,0,0,4096,6144,6144,6144,6144,6144,
6144,6144,6144,6144,6144,38912,2048,0,32768,48,
48,120,120,120,120,120,120,120,120,120,
48,48,0,0,0,0,0,0,512,768,
1792,1792,1792,1792,1920,34688,1920,1792,1792,1792,
33536,768,32768,0,32768,32768,32768,32768,32768,0,
32768,32768,0,32768,32768,32768,32768,0,32768,32768,
32768,33152,33728,960,34784,34784,34800,34800,34800,36848,
36856,36856,36856,36856,36856,36856,36856,36856,36860,36860,
36860,36860,36856,36856,36860,36856,36856,36856,36856,36856,
36856,36856,36856,34800,34800,34800,34784,33760,33760,33728,
33152,32768,32768,32768,32768,32768,32768,32768,32768,32800,
32880,32880,33008,33016,33016,33016,33016,33016,33016,33016,
33008,33008,33008,34544,34400,34368,34304,34560,34560,34560,
34560,34560,34560,34304,34304,34316,33820,32798,32798,32830,
32830,32830,32830,32830,33854,34366,34366,36382,36382,36380,
36636,36608,36608,36608,36608,36608,36608,36608,36608,36352}

/*

{192,192,64,64,64,0,0,0,0,0,
0,0,24,24,28,28,60,60,62,62,
62,62,62,62,62,62,62,62,62,62,
62,62,62,62,30,30,30,28,28,12,
12,0,0,0,0,0,0,0,0,0,
0,0,0,0,128,128,128,384,384,384,
384,384,384,384,384,384,384,384,384,128,
128,0,0,8,8,8,24,24,24,24,
8,8,8,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,32,
32,112,112,112,112,112,112,112,112,120,
120,120,120,112,112,112,112,112,112,112,
112,48,32,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,24,24,
24,24,24,24,24,24,24,24,24,24,
24,0,0,0,0,0,0,0,0,0,
0,0,16,24,56,56,56,60,60,60,
60,60,124,124,124,124,124,124,124,124,
124,124,60,60,60,60,60,56,56,56,
24,16,0,0,0,128,128,128,128,128,
128,128,128,128,128,128,128,0,0,8,
12,12,12,12,12,12,12,12,12,12,
12,8,0,0,0,0,0,0,64,96,
96,96,96,96,96,96,96,96,96,96,
96,96,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,48,112,112,120,120,120,120,124,252,
252,252,252,252,252,252,252,252,252,252,
254,252,252,252,252,252,252,252,252,252,
252,252,124,124,124,120,120,120,112,48,
0,0,0,0,0,0,0,0,0,8,
8,28,28,28,28,28,28,28,28,28,
28,28,88,88,88,64,64,96,96,96,
96,96,96,64,64,66,70,6,6,14,
15,15,15,15,79,79,78,198,198,198,
230,224,224,224,224,224,224,224,224,192}*/
};

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
  iut++;
  iut%=500;
  if(iut == 0) {
    ab ^= 1; //switch image buffer;
  }
  //digitalWrite(pin0, HIGH);
}

inline void WriteLetter1Width(int leds){
  PORTD = leds << 2;  //0 ~ 5 -> D2 ~ D7
  PORTB = leds >> 6;  //6 ~ 9 -> B0 ~ B3
  if(leds & 0x100) { //workaround for LED8(PB2), don't know why it doesn't work in last line of code
    digitalWrite(10, HIGH);
  }else{
    digitalWrite(10, LOW);
  }
  PORTC = leds >> 9;  //10~15 -> C０~ C5
  #ifndef USING_TIMER
    delayMicroseconds(ASP);
  #endif
}

int toggle = 0; 

ISR(TIMER1_COMPA_vect){
  if(startFrame) {
    asidx = 0;
    startFrame = false;
  }
  if(asidx < ANGLE_LINES) {
    WriteLetter1Width(data[ab][asidx]);
    asidx++;
  } else if(asidx == ANGLE_LINES) {
    WriteLetter1Width(0);
  }
  /*
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
  }*/
}

int inc = 0;

#ifdef USING_TIMER
inline void checkHome() {
  ht++;
  //state = digitalRead(U18);
  /*if(analogRead(U18) > 700) {
    state = HIGH;
  }else{
    state = LOW;
  }*/
  state = PINB & 0x10;
  if (state != lastState && state == 0){
    uint32_t home_period_t = ht - lht; //period in homing ticks
    lht = ht;
    //home_frequence = tick_requence/home_period_t --> home_period_t = tick_requence/home_frequence
    if ( home_period_t > 100 && home_period_t < 10000){ //between 1 ~ 100 hz, only accept freqence in then between
      //angle angle scanning frequence:
      //    asf = home_frequence * ANGLE_LINES 
      //        = ANGLE_LINES * tick_requence/home_period_t
      //        = ANGLE_LINES * 10000/home_period_t
      //OCR1A = 16*10^6/asf - 1
      //      = home_period_t * 16*10^6/(ANGLE_LINES * 10000) - 1
      //      = home_period_t * 1600/ANGLE_LINES - 1
      OCR1A =  home_period_t * 1600/ANGLE_LINES - 1;
      TCNT1  = 0;

    }
    startFrame = true;
  }
  lastState = state;
}

#else
inline void checkHome() {
  //state = digitalRead(U18);
  /*if(analogRead(U18) > 700) {
  if(PINB & 0x10) {
    state = HIGH;
  }else{
    state = LOW;
  }*/
  state = PINB & 0x10;
  if (state != lastState && state == 0){
    startFrame = true;
  }
  lastState = state;
}

#endif

ISR(TIMER2_COMPA_vect){
  checkHome();
  //digitalWrite(pin0+2, state);
  /*asidx++;
  asidx%=10000;
  if(asidx==0) {
    if(toggle) {
      digitalWrite(pin0+2, HIGH);
      toggle = 0;
    } else {
      digitalWrite(pin0+2, LOW);
      toggle = 1;
    }
  }*/
}

void setup() {
  for (int Pin = pin0; Pin <=pin9; Pin++) {
    pinMode(Pin, OUTPUT);
  }

  for (int Pin = pin10; Pin <=pin15; Pin++) {
    pinMode(Pin, OUTPUT);
  }
 
  pinMode(U18, INPUT);

#ifdef USING_TIMER
  cli();//stop interrupts
  init_timer0();
  init_timer1();
  init_timer2();
  sei();//allow interrupt
#endif
}

int k=0;
void loop() {
#ifndef USING_TIMER  
  //startFrame=true;
  if (startFrame) {
    startFrame = false;
    for(int j=0;j<360;j++){
      //WriteLetter1Width(pgm_read_word(&data[i][j]));
      WriteLetter1Width(data[ab][j]);
      //WriteLetter1Width(0x100);
      checkHome();
      if(startFrame) {
        break;  //stop current frame
      }
    }
    WriteLetter1Width(0);
    k++;
    if(k >= 200) {
      k = 0;
      ab ^= 1;
    }
  } else {
    checkHome();
  }
#endif
}

/*
inline void WriteLetter1Width(int leds){
  m=0;
  for (int Pin = pin1; Pin <=pin9; Pin++) {
    if (leds & (1 << m)) {
      digitalWrite(Pin, HIGH);
    }else {
      digitalWrite(Pin, LOW);
    }
    m=m+1;
  }

  if (leds & (1 << m)) {
    digitalWrite(pin10, HIGH);
  }else {
    digitalWrite(pin10, LOW);
  }
  
  //delayMicroseconds(timer);
}
*/



