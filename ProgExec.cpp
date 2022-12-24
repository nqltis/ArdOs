/*
  ProgExec.cpp : Library for 
  Program Execution on ArdOs
  nqltis - 23/12/2022
*/

#include "ProgExec.h"

ProgExec::ProgExec() {
  for (char i = 0; i < 4; i++) {
    currentCommand[i] = 0;
  }
  commandIndex = 0;
  commandLen = 0;
  acc = 0;
}

char ProgExec::getCommand() {
  return currentCommand[0];
}

char ProgExec::execute(char command) {
  if (!commandIndex) {  //initialization of new commands
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
  currentCommand[commandIndex++] = command;
  if (commandIndex <= commandLen) { //command incomplete
    return 0; //Ask for more arguments
  }
  switch (currentCommand[0]) {  //command execution
    default:
      if (currentCommand[0] < 0) return currentCommand[0];  //return syscall
  }
  commandIndex = 0;
  commandLen = 0;
  return 1;
}