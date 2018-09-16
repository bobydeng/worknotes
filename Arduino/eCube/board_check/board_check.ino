// avr-libc defines _NOP() since 1.6.2  
#ifndef _NOP  
#define _NOP() do { __asm__ volatile ("nop"); } while (0)  
#endif  

#define __UNO__  __UNO__

#ifdef __UNO__

#define DA164 2
#define STCP595  3
//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define CLK164 12
#define BLK9 13

//PIN2, PD2, DA164
#define dataUp() (PORTD |= 0x04)  
#define dataDown() (PORTD &= 0xFB)

//PIN3, PD3, STCP595
#define regClkUp() (PORTD |= 0x08)  
#define regClkDown() (PORTD &= 0xF7)

//PIN12, PB4, CLK164
#define dataClkUp() (PORTB |= 0x10)  
#define dataClkDown() (PORTB &= 0xEF)

//PIN13, PB5, BLK9
#define blk9Up() (PORTB |= 0x20)  
#define blk9Down() (PORTB &= 0xDF)

//line selection, each face has 3 lines for red, green and blue respatively
#define PORT_LINE PORTC   //A0 - A5 of Uno
#define DDR_LINE DDRC

#else

#define CLK164 2
#define DA164 3
#define STCP595  4
//hc164 is 8 bits shift register, there is 9 block on each face, 
//use below pin to control the ninth block directly
#define BLK9 5

//PIN2, PE4, CLK164
#define dataClkUp() (PORTE |= 0x10)  
#define dataClkDown() (PORTE &= 0xEF)

//PIN3, PE5, DA164
#define dataUp() (PORTE |= 0x20)  
#define dataDown() (PORTE &= 0xDF)

//PIN4, PG5, STCP595
#define regClkUp() (PORTG |= 0x20)  
#define regClkDown() (PORTG &= 0xDF)

//PIN5, PE3, BLK9
#define blk9Up() (PORTE |= 0x08)  
#define blk9Down() (PORTE &= 0xF7)

//line selection, each face has 3 lines for red, green and blue respatively
#define PORT_LINE PORTA   //PIN22 - PIN29 of Mega2560
#define DDR_LINE DDRA

#endif


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


void setup() {
  DDR_LINE = 0XFF;
  PORT_LINE = 23; //0xFF //turn off all

  pinMode(CLK164, OUTPUT);
  pinMode(DA164, OUTPUT);
  pinMode(BLK9, OUTPUT); 
  pinMode(STCP595,OUTPUT);
}

byte line=0;
int channel=0;
short color ;
int face=0;

void loop() {

  int line =   face * 3 + channel;
  if(line == 1 || line == 2){//% 3  == 1) {//|| line >2) {
      color = 0b111111111;
  } else {
    color = 0;
  }
  color = 0b111111111;
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
  face %= 6;

  if(face == 0) {
    channel++;
    channel %= 3;
  }
  delayMicroseconds(100);

}

