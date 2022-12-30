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
      LCDoutput(byte RS, byte RW, byte E, byte DataPins[8]);
      void drawchar(char chr, byte pos);
      void printScreen(char *line1, char *line2);
      void init();
    private:
      byte _rs;
      byte _rw;
      byte _e;
      byte _datapins[8];
      void pulse();
      void senddata(byte data);
      void execinstr(byte data);
      void dwrite(byte data);
};

#endif
