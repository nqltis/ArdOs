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
  cond = 0;
}

char ProgExec::getCommand() {
  return arg[0];
}

int ProgExec::getArg(char offset, unsigned char size) { //get argument of specified size and offset in bytes
  int _ans = 0;
  for(char i = offset + 1; i < offset + size + 1; i++) {
    char _arg;
    if (arg[i] & 128) {
      _arg = progmem[arg[i] & 127];
    } else {
      _arg = arg[i];
    }
    _ans = (_ans << 8) | _arg;
  }
  return _ans;
}

void ProgExec::getProgMem(char *output, unsigned char offset, unsigned char size) {
  for(unsigned char i = 0; i < size; i++) {
    output[i] = progmem[i + offset];
  }
}

char ProgExec::execute(char command) {
  if (!argIndex) {  //initialization of new commands
    switch (command) {
      case 0:   //NOP
        commandLen = 0;
      break;
      case 1:   //LD
        commandLen = 1;
      break;
      case 2:   //ST
        commandLen = 1;
      break; 
      case 3:   //AND
        commandLen = 1;
      break; 
      case 4:   //OR
        commandLen = 1;
      break; 
      case 5:   //NOT
        commandLen = 0;
      break; 
      case 6:   //XOR
        commandLen = 1;
      break; 
      case 7:   //ADD
        commandLen = 1;
      break;
      case 8:   //SUB
        commandLen = 1;
      break; 
      case 9:   //MUL
        commandLen = 1;
      break; 
      case 10:  //DIV
        commandLen = 1;
      break; 
      case 11:  //MOD
        commandLen = 1;
      break; 
      case 12:  //GT
        commandLen = 1;
      break; 
      case 13:  //EQ
        commandLen = 1;
      break; 
      case 14:  //LT
        commandLen = 1;
      break;
      case -1:  //ext
        commandLen = 0;
      break;
      case -2:  //slp I/R I/R
        commandLen = 2; //waiting for two more arguments
      break;
      case -3:  //pch I/R I/R
        commandLen = 2;
      break;
      case -4:  //pst I/R I/R
        commandLen = 2;
      break;
      case -64: //set loop start
        commandLen = 0;
      break;
      case -63: //loop
        commandLen = 0;
      break;
    }
  } 
  arg[argIndex++] = command;
  if (argIndex <= commandLen) { //command incomplete
    return 0; //Ask for more arguments
  }
  switch (arg[0]) {  //command execution
    case 1:   //LD
      acc = getArg(0, 1);
    break;
    case 2:   //ST
      progmem[getArg(0, 1)] = acc;
    break;
    case 3:   //AND
      acc &= getArg(0, 1);
    break;
    case 4:   //OR
      acc |= getArg(0, 1);
    break;
    case 5:   //NOT
      acc = ~acc;
    break;
    case 6:   //XOR
      acc ^= getArg(0, 1);
    break;
    case 7:   //ADD
      acc += getArg(0, 1);
    break;
    case 8:   //SUB
      acc -= getArg(0, 1);
    break;
    case 9:   //MUL
      acc *= getArg(0, 1);
    break;
    case 10:  //DIV
      acc /= getArg(0, 1);
    break;
    case 11:  //MOD
      acc %= getArg(0, 1);
    break;
    case 12:  //GT
      cond = (acc > getArg(0, 1));
    break;
    case 13:  //MOD
      cond = (acc = getArg(0, 1));
    break;
    case 14:  //MOD
      cond = (acc < getArg(0, 1));
    break;
    default:
      if (arg[0] < 0) { //syscall
        argIndex = 0;
        commandLen = 0;
        return arg[0];  //return syscall
      }  
  }
  argIndex = 0;
  commandLen = 0;
  return 1;
}