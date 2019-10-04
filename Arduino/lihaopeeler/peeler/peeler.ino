
#include <Servo.h>

const int valvePin = 6;
const int btnPin = A0;

const int door_o_angle=45;
const int door_c_angle=0;
const int pipe_load_angle=90;
const int pipe_unload_angle=146;

Servo pipe_servo;
Servo door_servo;

void setup() {
  pinMode(valvePin, OUTPUT);
  pinMode(btnPin, INPUT);
  pipe_servo.attach(8);
  door_servo.attach(9);
}

void loop() {
  int btn = digitalRead(btnPin);
  if(btn) {
    //pipe_servo.write(90);
    pipe_servo.write(pipe_unload_angle);
    delay(500);
    door_servo.write(door_c_angle);
    delay(500);
    
    blow();
  
    door_servo.write(door_o_angle);
    delay(500);
  
    blow();
  
    pipe_servo.write(pipe_load_angle);
    delay(500);
  }
}


void blow() {
  digitalWrite(valvePin, HIGH);
  delay(500);
  digitalWrite(valvePin, LOW);
}

