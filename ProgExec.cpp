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
      _ans = (_ans << 8) | _arg;
    } else {
      _arg = arg[i];
      _ans = (_ans << 7) | _arg;
    }
  }
  return _ans;
}

void ProgExec::getProgMem(char *output, unsigned char offset, unsigned char size) {
  for(unsigned char i = 0; i < size; i++) {
    output[i] = progmem[i + offset];
  }
}

void ProgExec::putMem(char value, unsigned char address) {
  progmem[address] = value;
}

void ProgExec::setCommandLen(char command) {
  switch (command) {
    case 0:   //NOP
      commandLen = 0;
    break;
    case 1:   //LD
      commandLen = 1;
    break;
    case 20:  //LD_
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
    case 12:  //EQ
      commandLen = 1;
    break; 
    case 13:  //GT
      commandLen = 1;
    break; 
    case 14:  //LT
      commandLen = 1;
    break;
    case 15:  //GE
      commandLen = 1;
    break;
    case 16:  //LE
      commandLen = 1;
    break;
    case 17:  //JMP
      commandLen = 1;
    break;
    case 18:  //JMPC
      commandLen = 1;
    break;
    case 19:  //JMPNC
      commandLen = 1;
    break;
    case -1:  //ext
      commandLen = 0;
    break;
    case -2:  //slp I/R I/R
      commandLen = 2; //waiting for two more arguments
    break;
    case -3:  //lab I
      commandLen = 1;
    break;
    case -4:  //jmp I
      commandLen = 1;
    break;
    case -5:  //ldm I I
      commandLen = 2;
    break;
    case -6:  //pch I/R I/R
      commandLen = 2;
    break;
    case -7:  //pst I/R I/R
      commandLen = 2;
    break;
    case -10: //gst I I
      commandLen = 2;
    break;
  }
}

char ProgExec::ignore(char command) { //same as execute() but doesn't execute 
  if (!argIndex) setCommandLen(command); //initialization of new commands
  arg[argIndex++] = command;
  if (argIndex <= commandLen) return 0; //Command incomplete : ask for more arguments
  argIndex = 0;
  commandLen = 0;
  if (arg[0] < 0) return arg[0];  //if syscall, return syscall
  return 1; //Instruction complete
}

char ProgExec::execute(char command) {
  if (!argIndex) setCommandLen(command); //initialization of new commands
  if (arg[0] != 127) arg[argIndex] = command; //Load byte unless if jumping (because argIndex can exceed arg size);
  argIndex++;
  if (argIndex <= commandLen) return 0; //Command incomplete : ask for more arguments
  switch (arg[0]) {  //command execution
    case 1:   //LD
      acc = getArg(0, 1);
    break;
    case 20:  //LD_
      acc = progmem[progmem[getArg(0, 1)]];
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
    case 12:  //EQ
      cond = (acc == getArg(0, 1));
    break;
    case 13:  //GT
      cond = (acc > getArg(0, 1));
    break;
    case 14:  //LT
      cond = (acc < getArg(0, 1));
    break;
    case 15:  //GE
      cond = (acc >= getArg(0, 1));
    break;
    case 16:  //LE
      cond = (acc <= getArg(0, 1));
    break;
    case 17:  //JMP
      arg[0] = 127; //Create an idle instruction
      argIndex = 1;
      commandLen = getArg(0, 1);  //Wait for as many args as requested jumps
      return 0; //Ask for more arguments (which will not be read)
    break;
    case 18:  //JMPC
      if (cond) {
        arg[0] = 127; //Create an idle instruction
        argIndex = 1;
        commandLen = getArg(0, 1);  //Wait for as many args as requested jumps
        return 0; //Ask for more arguments (which will not be read)
      }
    break;
    case 19:  //JMPNC
      if (!cond) {
        arg[0] = 127; //Create an idle instruction
        argIndex = 1;
        commandLen = getArg(0, 1);  //Wait for as many args as requested jumps
        return 0; //Ask for more arguments (which will not be read)
      }
    break;
    case 127: //end of JMP
      arg[0] = 0; //reset arg[0] to allow instruction loading again
    break;
    default:
      if (arg[0] < 0) { //syscall
        argIndex = 0;
        commandLen = 0;
        return arg[0];  //return syscall
      }
    break;
  }
  argIndex = 0;
  commandLen = 0;
  return 1;
}