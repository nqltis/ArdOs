/*
  LCDoutput.h : Library for controlling 
  a 2 by 16 LCD screen .
  nqltis - 18/10/2022
*/

#include "Arduino.h"
#include "LCDoutput.h"

LCDoutput::LCDoutput(const byte RS, const byte RW,const byte E,const byte DataPins[8]) {
  for (byte i = 0; i < 8; i++) {
    _datapins[i] = DataPins[i];
  }
  _rs = RS;
  _rw = RW;
  _e = E;
  pinMode(_rs, OUTPUT);
  pinMode(_rw, OUTPUT);
  pinMode(_e, OUTPUT);
  digitalWrite(_rs, LOW);
  digitalWrite(_rw, LOW);
  digitalWrite(_e, LOW);
  for (byte i = 0; i < 8; i++) {
    pinMode(_datapins[i], OUTPUT);
    digitalWrite(_datapins[i], LOW);
  }
}

void LCDoutput::init() {
  execinstr(1);    //Clear the screen
  execinstr(2);    //Return home
  execinstr(6);    //Entry mode
  execinstr(14);   //Display on, Cursor on
  execinstr(20);   //Enable autoshift right
  execinstr(58);   //Function set
  execinstr(12);   //Cursor off
}

void LCDoutput::pulse() {
  delay(1);
  digitalWrite(_e, HIGH);
  delay(1);
  digitalWrite(_e, LOW);
  delay(1);
}

void LCDoutput::dwrite(byte data) {
  for (byte i = 0; i < 8; i++) {
    digitalWrite(_datapins[i], !!(data & (1 << i)));
  }
}

void LCDoutput::execinstr(byte data) {
  digitalWrite(_rs, LOW);
  dwrite(data);
  pulse();
}

void LCDoutput::senddata(byte data) {
  digitalWrite(_rs, HIGH);
  dwrite(data); 
  pulse();
}

void LCDoutput::drawchar(char chr, byte pos) {
  if (pos >= 16) pos += 48;
  execinstr(128 | pos);
  senddata(chr);
}

void LCDoutput::printScreen(char *line1, char *line2) {
  execinstr(128);
  char endOfString = 0;
  for (byte i = 0; i < 16; i++) { //Line 1
    if (!endOfString && (line1[i] == 0)) endOfString = 1;
    if (!endOfString) senddata(line1[i]);
    else senddata(32);  //Fill with space
  }
  execinstr(192);
  endOfString = 0;
  for (byte i = 0; i < 16; i++) { //Line 2
    if (!endOfString && (line2[i] == 0)) endOfString = 1;
    if (!endOfString) senddata(line2[i]);
    else senddata(32);  //Fill with space
  }
}
