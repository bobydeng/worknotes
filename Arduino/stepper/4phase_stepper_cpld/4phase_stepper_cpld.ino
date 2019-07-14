
const int clk1 = 2;
const int dir1 = 3;

const int clk2 = 4;
const int dir2 = 5;

const int steps = 4096;
const int pulse_width = 800;

void _setup(int clk, int dir) {
  pinMode(clk, OUTPUT);
  pinMode(dir, OUTPUT);  
  digitalWrite(clk, LOW);
  digitalWrite(dir, LOW);
}

void setup() {
  _setup(clk1,dir1);
  _setup(clk2,dir2);  
}

void pulse_up() {
  digitalWrite(clk1, HIGH);
  digitalWrite(clk2, HIGH);
}

void pulse_down() {
  digitalWrite(clk1, LOW);
  digitalWrite(clk2, LOW);
}

void onestep() {
  pulse_up();
  delayMicroseconds(pulse_width);
  pulse_down();
  delayMicroseconds(pulse_width);
}

void loop() {
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);
  for(int i=0;i<steps;i++) {
    onestep();
  }
  delay(2000);
  digitalWrite(dir1, HIGH);
  digitalWrite(dir2, HIGH);
  for(int i=0;i<steps;i++) {
    onestep();
  }
  delay(2000);
}
