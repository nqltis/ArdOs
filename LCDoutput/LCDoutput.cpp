/*
  LCDoutput.h : Library for controlling 
  a 2 by 16 LCD screen .
  nqltis - 18/10/2022
*/

#include "Arduino.h"
#include "LCDoutput.h"

LCDoutput::LCDoutput(int RS, int RW, int E, byte DataPins[8]) {
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
  digitalWrite(E, HIGH);
  delay(1);
  digitalWrite(E, LOW);
  delay(1);
}

void LCDoutput::dwrite(byte data) {
  for (byte i = 0; i < 8; i++) {
    digitalWrite(DataPin[i], !!(data & (1 << i)));
  }
}

void LCDoutput::execinstr(byte data) {
  digitalWrite(RS, LOW);
  dwrite(data);
  pulse();
}

void LCDoutput::senddata(byte data) {
  digitalWrite(RS, HIGH);
  dwrite(data);
  pulse();
}

void LCDoutput::drawchar(char chr, int pos) {
  if (pos >= 16) pos+= 48;
  execinstr(128 | pos);
  senddata(chr);
}

void LCDoutput::printScreen(String line1, String line2) {
  line1 += "                ";
  line2 += "                ";
  execinstr(128);
  for (byte i = 0; i < 16; i++) {
    senddata(line1[i]);
  }
  execinstr(192);
  for (byte i = 0; i < 16; i++) {
    senddata(line2[i]);
  }
}