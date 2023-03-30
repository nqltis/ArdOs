/*
  M16input.h : Library for controlling 
  a 16-input matrix keyboard with 8 connectors.
  nqltis - 18/10/2022
*/

#ifndef M16input_h
#define M16input_h

#include <Arduino.h>

class M16input
{
    public:
      M16input(const byte InputPins[8]);
      char button();
    private:
      byte inRead();
      char translate(byte inbyte);
      byte _oldinput;
      byte _newinput;
      byte _inputpins[8];
};

#endif