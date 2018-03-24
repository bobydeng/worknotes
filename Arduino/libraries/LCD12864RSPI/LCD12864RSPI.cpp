//Demo LCD12864 spi
//www.dfrobot.com
//edit by savage at 2014-02-28

#include "LCD12864RSPI.h"


#include "Arduino.h"
#include <Wire.h> 
#include <inttypes.h>
#include <string.h> //needed for strlen()
#include <avr/pgmspace.h>



LCD12864RSPI::LCD12864RSPI(int _latchPin,int _dataPin,int _clockPin):
	latchPin(_latchPin),dataPin(_dataPin),clockPin(_clockPin)
{

    pinMode(latchPin, OUTPUT);     
    pinMode(clockPin, OUTPUT);    
    pinMode(dataPin, OUTPUT);
    digitalWrite(latchPin, LOW);
    WriteCommand(0x30);        //功能设定控制字
    WriteCommand(0x0c);        //显示开关控制字
    WriteCommand(0x01);        //清除屏幕控制字
    WriteCommand(0x06);        //进入设定点控制字

} 

//重写write
inline size_t LCD12864RSPI::write(uint8_t value) {
	WriteData(value);
}


void LCD12864RSPI::WriteByte(uint8_t dat)
{
    digitalWrite(latchPin, HIGH);
    shiftOut(dataPin, clockPin, MSBFIRST, dat);
    digitalWrite(latchPin, LOW);
}


void LCD12864RSPI::WriteCommand(uint8_t CMD)
{
   int H_data,L_data;
   H_data = CMD;
   H_data &= 0xf0;           //屏蔽低4位的数据
   L_data = CMD;             //xxxx0000格式
   L_data &= 0x0f;           //屏蔽高4位的数据
   L_data <<= 4;             //xxxx0000格式
   WriteByte(0xf8);          //RS=0，写入的是指令；
   WriteByte(H_data);
   WriteByte(L_data);
}


void LCD12864RSPI::WriteData(uint8_t CMD)
{
   int H_data,L_data;
   H_data = CMD;
   H_data &= 0xf0;           //屏蔽低4位的数据
   L_data = CMD;             //xxxx0000格式
   L_data &= 0x0f;           //屏蔽高4位的数据
   L_data <<= 4;             //xxxx0000格式
   WriteByte(0xfa);          //RS=1，写入的是数据
   WriteByte(H_data);
   WriteByte(L_data);
}


void LCD12864RSPI::clear(void)
{  
    WriteCommand(0x30);//
    WriteCommand(0x01);//清除显示
    delay(1);//指令需要时间反应
}

void LCD12864RSPI::chinese(uchar *ptr,int dat)
{
  int i;
  for(i=0;i<dat;i++)
    { 
      WriteData(ptr[i]);//显示汉字时注意码值，连续两个码表示一个汉字
    }
}



void LCD12864RSPI::setCursor(uint8_t X,uint8_t Y)
{
  switch(X)
   {
     case 0:  Y|=0x80;break;

     case 1:  Y|=0x90;break;

     case 2:  Y|=0x88;break;

     case 3:  Y|=0x98;break;

     default: break;
   }
  WriteCommand(Y); // 定位显示起始地址
}


void LCD12864RSPI::DrawFullScreen(uchar *p)
{
      int ygroup,x,y,i;
      int temp;
      int tmp;
             
      for(ygroup=0;ygroup<64;ygroup++)           //写入液晶上半图象部分
        {                           //写入坐标
           if(ygroup<32)
            {
             x=0x80;
             y=ygroup+0x80;
            }
           else 
            {
              x=0x88;
              y=ygroup-32+0x80;    
            }         
           WriteCommand(0x34);        //写入扩充指令命令
           WriteCommand(y);           //写入y轴坐标
           WriteCommand(x);           //写入x轴坐标
           WriteCommand(0x30);        //写入基本指令命令
           tmp=ygroup*16;
           for(i=0;i<16;i++)
		 {
		    temp=p[tmp++];
		    WriteData(temp);
               }
          }
        WriteCommand(0x34);        //写入扩充指令命令
        WriteCommand(0x36);        //显示图象
}
