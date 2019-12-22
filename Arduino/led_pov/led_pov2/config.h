#ifndef __CONFIG__
#define __CONFIG__

#define USING_TIMER __using_timer__
//angle resolution
#define SCR_ANGLE_RES 200

//#define __DEBUG__

typedef unsigned char uint8;

//#define __TEST__

#ifndef __TEST__
#include <Arduino.h>
#else
    #include <math.h>
    #include <string.h>
    #include <stdlib.h>
    #include <iostream>

    #define OUTPUT 1
    #define INPUT 0
    #define HIGH 1
    #define LOW 0

    #define PROGMEM

    typedef unsigned char uint8_t;
    typedef unsigned short uint16;

    extern void pinMode(int a, int b);
    extern void digitalWrite(int a, int b);
    extern uint16 pgm_read_word(const void*);
    extern void delay(int);

    #define memcpy_P(d,s, size) memcpy(d, s, size)

    extern int PORTB, PORTC, PORTD;

#endif


#endif
