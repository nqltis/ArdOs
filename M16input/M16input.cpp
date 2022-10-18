/*
  M16input.h : Library for controlling 
  a 16-input matrix keyboard with 8 connectors.
  nqltis - 18/10/2022
*/

#include "Arduino.h"
#include "M16input.h"

M16input::M16input(byte InputPins[8])
{
  for (byte i = 0; i < 8; i++) {
    _inputpins[i] = InputPins[i];
  }
  for (byte i = 0; i < 4; i++) {
    pinMode(_inputpins[i], OUTPUT);
    pinMode(_inputpins[i + 4], INPUT_PULLUP);
    digitalWrite(_inputpins[i], HIGH);
  }
}



byte M16input::inRead() {
  byte input = 0;
  for (byte i = 0; i < 4; i++) {
    digitalWrite(_inputpins[i], LOW);
    delay(1);
    for (byte j = 0; j < 4; j++) {
      if (!digitalRead(_inputpins[j + 4])) {
        input = input | (16 << i) | (1 << j);
      }
    }
    digitalWrite(_inputpins[i], HIGH);
    delay(1);
  }
  return input;
}

char M16input::translate(byte inbyte) {
  switch (inbyte) {
    case 0: return ' ';
    case 17: return '1';
    case 18: return '2';
    case 20: return '3';
    case 24: return 'A';
    case 33: return '4';
    case 34: return '5';
    case 36: return '6';
    case 40: return 'B';
    case 65: return '7';
    case 66: return '8';
    case 68: return '9';
    case 72: return 'C';
    case 129: return '*';
    case 130: return '0';
    case 132: return '#';
    case 136: return 'D';
  }
  return 'n';
}

char M16input::button() { //Returns the pressed button on the first cycle of its activation
  _newinput = inRead();
  if ((_oldinput == 0) && (_newinput != 0)) {
    _oldinput = _newinput;
    return translate(_newinput);
  }
  _oldinput = _newinput;
  return 0;
}