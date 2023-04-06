/*
  M16input.h : Library for controlling 
  a 16-input matrix keyboard with 8 connectors.
  nqltis - 18/10/2022
*/

#include <Arduino.h>
#include "M16input.h"

M16input::M16input(const byte OutputPins[2], const byte InputPins[4]) {
  for (byte i = 0; i < 4; i++) { //Save pins in a private array
    _inputpins[i] = InputPins[i];
  }
  _outputpins[0] = OutputPins[0];
  _outputpins[1] = OutputPins[1];
  for (byte i = 0; i < 4; i++) {  //Set pins mode
    pinMode(_inputpins[i], INPUT_PULLUP);
  }
  pinMode(_outputpins[0], OUTPUT);
  pinMode(_outputpins[1], OUTPUT);
  digitalWrite(_outputpins[0], LOW);
  digitalWrite(_outputpins[1], LOW);
}

/* 4 MSB are outputs, 4 LSB are inputs.
This function tests every combination to detect every
connection inside the key pad. A high bit means a
connection on the pin's wire */
byte M16input::inRead() { 
  byte input = 0;
  for (byte i = 0; i < 4; i++) {  //for each output
    digitalWrite(_outputpins[0], i & 1);  //Select demultiplexer output
    digitalWrite(_outputpins[1], i & 2);
    delay(1);
    for (byte j = 0; j < 4; j++) {  //test every input for connection
      if (!digitalRead(_inputpins[j])) {
        input = input | (16 << i) | (1 << j); //OR-write result if connection
      }
    }
  }
  return input;
}

char M16input::translate(byte inbyte) { //Link inRead() output to corresponding key
  switch (inbyte) {
    case 0: return ' '; //Currently never used
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
  return 'n'; //Return 'n' if unknown byte
}

char M16input::button() { //Returns the pressed button on the first cycle of its activation
  _newinput = inRead();
  if ((_oldinput == 0) && (_newinput != 0)) { //if input used to be null but isn't anymore
    _oldinput = _newinput;
    return translate(_newinput);
  }
  _oldinput = _newinput;
  return 0;
}
