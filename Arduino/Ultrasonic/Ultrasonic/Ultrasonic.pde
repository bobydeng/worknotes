/*
  Make sure the Ultrasonic Library is installed to your arduino software.
  (You can do this by copying the folder "Ultrasonic" which includes:
    Folder: "examples" including a demo
    "keywords.txt."
    "ultrasonic.cpp"
    "ultrasonic.h"
  to the libraries folder of Arduino)
   
  Note: This tutorial and source code will be revised in June. 
   
  Watch tutorial: http://www.youtube.com/watch?v=EfU6SlrOndc
*/

#include <Ultrasonic.h>
//#include "Ultrasonic.h"

int SPEAKER = 8; // Piezo Speaker Pin

int LED1 = 9;  // LED1 Pin
int LED2 = 10; // LED2 Pin

int TRIG = 5; // Trigger Pin
int ECHO = 4; // Echo Pin

int Counter = 1; // To count the number of beeps.

int Range; // The range of the object from the HC-SR04 Ultarsonic Module
boolean Alarm; // To test if the alarm should play or not.
  
Ultrasonic ultrasonic(TRIG,ECHO); // Create and initialize the Ultrasonic object.

void setup() {
 
  Serial.begin(9600); 
  pinMode(LED1, OUTPUT); 
  pinMode(LED2, OUTPUT); 
  
}

void loop() {
  
  Range = ultrasonic.Ranging(CM); // Range is calculated in Centimeters. 
  // Range = ultrasonic.Ranging(INC); // Range is calculated in Inches.
 
  Serial.print(Range);   
  Serial.println(" cm"); 
  
  if (Range < 10) { 
    Alarm = true;   
    Counter = 1;    
  }
  else if (Counter == 11) { 
    Alarm = false;          
    Counter = 1;
  }
  
  if (Alarm)
    AlarmOn();
  
}

void AlarmOn(){
  
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
  tone(SPEAKER, 1000);
  delay(100);
  
  noTone(SPEAKER); 
  
  digitalWrite(LED2, HIGH);
  digitalWrite(LED1, LOW);
  delay(100);
  
  digitalWrite(LED2, LOW);
  digitalWrite(LED1, LOW);
  
  Counter++;
  
}
