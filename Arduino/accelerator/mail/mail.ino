#include <LCD12864RSPI.h>
#include <Wire.h>

#define Register_ID 0;
#define Register_2D 0x2D
#define Register_X0 0x32
#define Register_X1 0x33
#define Register_Y0 0x34
#define Register_Y1 0x35
#define Register_Z0 0x36
#define Register_Z1 0x37

#define ADXAddress (0xA7 >> 1)

int readAxis(byte reg0, byte reg1);

int reading = 0;
int val = 0;
int x_out;
int y_out;
int z_out;
double xg,yg,zg;

LCD12864RSPI lcd(8,9,3);

void setup() {
  Wire.begin();
  delay(100);
  Wire.beginTransmission(ADXAddress);
  Wire.write(Register_2D);
  Wire.write(8);
  Wire.endTransmission();

}

void loop() {
  char str_buf[6];

  x_out = readAxis(Register_X0,Register_X1);
  y_out = readAxis(Register_Y0,Register_Y1);
  z_out = readAxis(Register_Z0,Register_Z1);

  xg = x_out/256.0;
  yg = y_out/256.0;
  zg = z_out/256.0;
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("x=");
  //dtostrf(xg,5,2,str_buf);
  lcd.print(xg);
  
  lcd.setCursor(1,0);
  lcd.print("y=");
  //dtostrf(yg,5,2,str_buf);
  lcd.print(yg);
  
  lcd.setCursor(2,0);
  lcd.print("z=");
  //dtostrf(zg,5,2,str_buf);
  lcd.print(zg);    

  delay(300);
  
}

int readAxis(byte reg0, byte reg1) {
  int d0,d1;
  Wire.beginTransmission(ADXAddress);
  Wire.write(reg0);
  Wire.write(reg1);
  Wire.endTransmission();
  Wire.requestFrom(ADXAddress,2);
  if(Wire.available()<=2); //wait
  {
    d0 = Wire.read();
    d1 = Wire.read();
  }
  return d0 + ( d1 << 8);
}

