//Demo LCD12864 spi
//www.dfrobot.com
//rewite by savage at 2014-02-28
//rewite by savage at 2014-03-01
#include <Arduino.h>
#ifndef LCD12864RSPI_h
#define LCD12864RSPI_h
#include <inttypes.h>
#include <avr/pgmspace.h>
#include "Print.h" 

class LCD12864RSPI :public Print{
	
typedef unsigned char uchar;

public:
LCD12864RSPI(int _latchPin,int _dataPin,int _clockPin);

void clear(void);
void chinese(uchar *ptr,int dat);
void DrawFullScreen(uchar *p);
void setCursor(uint8_t, uint8_t); 

private:
	virtual size_t write(uint8_t);
	void WriteCommand(uint8_t CMD);
  void WriteData(uint8_t CMD);
	void WriteByte(uint8_t dat);
	const int latchPin; 
  const int dataPin;  
  const int clockPin; 
};
//extern LCD12864RSPI LCDA;    
#endif
