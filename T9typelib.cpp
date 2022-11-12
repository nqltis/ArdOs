/*
  T9typelib.h : Library for string input 
  with a 9 buttons keypad like an old phone.
  nqltis - 05/11/2022
*/

#include "T9typelib.h"

TypeSession::TypeSession() {  //each time a string input is needed, a new session must be created
  inputStr[0] = 0;
  _index = -1;
  _activeKey = 0; 
  _cycleStep = 0;
}

int TypeSession::keyToNum(char key) {
  switch (key) {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
  }
  return -1;
}

void TypeSession::enterKey(char key) {
  const char mapping[10][5] = {
    {',', '.', '!', '?', 0},  //0 : ,.!?
    {' ', '-', '_', '/', 0},  //1 :  -_/
    { 'a', 'b', 'c', 0, 0},   //2 : abc
    { 'd', 'e', 'f', 0, 0},   //3 : def
    { 'g', 'h', 'i', 0, 0},   //.....
    { 'j', 'k', 'l', 0, 0},
    { 'm', 'n', 'o', 0, 0},
    { 'p', 'q', 'r', 's', 0},
    { 't', 'u', 'v', 0, 0},
    { 'w', 'x', 'y', 'z', 0}
  };
  if (key != _activeKey) {    //if new key:
    if (_index >= 15) return; //if buffer full, do nothing
    _activeKey = key; 
    _index++;         //go next slot
    _cycleStep = 0;   //reset cycling
    inputStr[_index] = mapping[keyToNum(key)][0]; //print corresponding char
    inputStr[_index + 1] = 0;   //Complete string with null char
  } else {    //same key, cycle through keys
    //_activeKey : same
    //_index : same
    _cycleStep++;
    if (!mapping[keyToNum(key)][_cycleStep]) _cycleStep = 0; //cycle through if no next character
    inputStr[_index] = mapping[keyToNum(key)][_cycleStep];
    inputStr[_index + 1] = 0; //complete string with null char
  }
}

void TypeSession::chgCase() {
  const char mapping[10][5] = {
    {',', '.', '!', '?', 0},  //0 : ,.!?
    {' ', '-', '_', '/', 0},  //1 :  -_/
    { 'a', 'b', 'c', 0, 0},   //2 : abc
    { 'd', 'e', 'f', 0, 0},   //3 : def
    { 'g', 'h', 'i', 0, 0},   //.....
    { 'j', 'k', 'l', 0, 0},
    { 'm', 'n', 'o', 0, 0},
    { 'p', 'q', 'r', 's', 0},
    { 't', 'u', 'v', 0, 0},
    { 'w', 'x', 'y', 'z', 0}
  };
  const char majmapping[10][5] = {
    {',', '.', '!', '?', 0},  //0 : ,.!?
    {' ', '-', '_', '/', 0},  //1 :  -_/
    { 'A', 'B', 'C', 0, 0},   //2 : abc
    { 'D', 'E', 'F', 0, 0},   //3 : def
    { 'G', 'H', 'I', 0, 0},   //.....
    { 'J', 'K', 'L', 0, 0},
    { 'M', 'N', 'O', 0, 0},
    { 'P', 'Q', 'R', 'S', 0},
    { 'T', 'U', 'V', 0, 0},
    { 'W', 'X', 'Y', 'Z', 0}
  };
  if (mapping[keyToNum(_activeKey)][_cycleStep] == inputStr[_index]) {
    inputStr[_index] = majmapping[keyToNum(_activeKey)][_cycleStep];
  } else {
    inputStr[_index] = mapping[keyToNum(_activeKey)][_cycleStep];
  }

}

void TypeSession::nextChar() {
  _activeKey = 0; //reset active key to print next key on next slot
}

void TypeSession::eraseChar() {
  inputStr[_index] = ' '; //write a blank space on active slot
  _index--; //go back one slot
}
