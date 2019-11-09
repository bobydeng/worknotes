
#include <Servo.h>

const int loadValvePin = 6;
const int unloadValvePin = 7;
const int btnPin = A0;

const int door_o_angle=105;
const int door_c_angle=48;
const int pipe_load_angle=45;
const int pipe_unload_angle=74;

Servo pipe_servo;
Servo door_servo;

void setup() {
  pinMode(loadValvePin, OUTPUT);
  pinMode(unloadValvePin, OUTPUT);
  pinMode(btnPin, INPUT);
  pipe_servo.attach(8);
  door_servo.attach(9);
  
  door_servo.write(door_o_angle);
  delay(500);
  pipe_servo.write(pipe_load_angle);
}

void loop() {
  int btn = digitalRead(btnPin);
  if(btn) {
    blow(loadValvePin);
    delay(1000);
    
    //pipe_servo.write(90);
    pipe_servo.write(pipe_unload_angle);
    delay(500);
    
    door_servo.write(door_c_angle);
    delay(500);
    
    blow(unloadValvePin);
  
    door_servo.write(door_o_angle);
    delay(500);
  
    blow(unloadValvePin);
  
    pipe_servo.write(pipe_load_angle);
    delay(500);
  }
}


void blow(int valve) {
  digitalWrite(valve, HIGH);
  delay(200);
  digitalWrite(valve, LOW);
}

