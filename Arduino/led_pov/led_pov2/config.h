#ifndef __CONFIG__
#define __CONFIG__

#include <Arduino.h>

#define USING_TIMER __using_timer__
//angle resolution
#define SCR_ANGLE_RES 200

typedef unsigned char uint8;

//#define __TEST__

#ifdef __TEST__

    #include <math.h>
    #include <string.h>
    #include <stdlib.h>

    #define OUTPUT 1
    #define INPUT 0
    #define HIGH 1
    #define LOW 0

    typedef unsigned char uint8_t;

    extern void pinMode(int a, int b);
    extern void digitalWrite(int a, int b);
    extern int pgm_read_word(const uint8_t*);
    extern void * memcpy_P(void*, const void*, int);

    extern int PORTB, PORTC, PORTD;

#endif


#endif
