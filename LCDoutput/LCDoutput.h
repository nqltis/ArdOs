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
      drawchar(char chr, byte pos);
      printScreen(String line1, String line2);
    private:
      byte _rs;
      byte _rw;
      byte _e;
      byte _datapins[8];
      pulse();
      senddata(byte data);
      execinstr(byte data);
      dwrite(byte data);
};

#endif