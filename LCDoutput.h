/*
  LCDoutput.h : Library for controlling 
  a 2 by 16 LCD screen .
  nqltis - 18/10/2022
*/

#ifndef LCDoutput_h
#define LCDoutput_h

#include <Arduino.h>

class LCDoutput
{
    public:
      LCDoutput(const byte RS, const byte E, const byte LCDDATA, const byte LCDCLK);
      void drawchar(char chr, byte pos);
      void printScreen(char *line1, char *line2);
      void init();
    private:
      byte _rs;
      byte _e;
      byte _data;
      byte _clk;
      void pulse();
      void senddata(byte data);
      void execinstr(byte data);
      void dwrite(byte data);
};

#endif
