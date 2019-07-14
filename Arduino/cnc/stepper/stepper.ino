
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

const int stepsPerRevolution = 4096;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

const int buttonPin1 = 2;
const int buttonPin2 = 3;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void setup() {
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);  
  // set the speed at 60 rpm:
  myStepper.setSpeed(11);//(12);
  // initialize the serial port:
  Serial.begin(9600);
}

void loop() {

  buttonState = digitalRead(buttonPin1);
  if(buttonState == HIGH) {
    myStepper.step(1);
    //delay(500);
  } else {
    buttonState = digitalRead(buttonPin2);
    if(buttonState == HIGH) {
      myStepper.step(-1);
      //delay(500);      
    }
  }
}

