/*
  ProgExec.cpp : Library for 
  Program Execution on ArdOs
  nqltis - 23/12/2022
*/

#include "ProgExec.h"

ProgExec::ProgExec() {
  for (char i = 0; i < 4; i++) {
    arg[i] = 0;
  }
  argIndex = 0;
  commandLen = 0;
  acc = 0;
}

char ProgExec::getCommand() {
  return arg[0];
}

int ProgExec::getArg(char offset, unsigned char size) { //get argument of specified size and offset in bytes
  int _ans = 0;
  for(char i = offset + 1; i < offset + size + 1; i++) {
    _ans = (_ans << 8) | arg[i];
  }
  return _ans;
}

char ProgExec::execute(char command) {
  if (!argIndex) {  //initialization of new commands
    switch (command) {
      case -1: //ext
        commandLen = 0;
      break;
      case -2: //slp I/R I/R
        commandLen = 2; //waiting for two more arguments
      break;
      case -3: //pch I/R I/R
        commandLen = 2;
      break;
    }
  } 
  arg[argIndex++] = command;
  if (argIndex <= commandLen) { //command incomplete
    return 0; //Ask for more arguments
  }
  switch (arg[0]) {  //command execution
    default:
      if (arg[0] < 0) {
        argIndex = 0;
        commandLen = 0;
        return arg[0];  //return syscall
      }  
  }
  argIndex = 0;
  commandLen = 0;
  return 1;
}