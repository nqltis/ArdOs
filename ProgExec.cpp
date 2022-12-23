/*
  ProgExec.cpp : Library for 
  Program Execution on ArdOs
  nqltis - 23/12/2022
*/

#include "ProgExec.h"

ProgExec::ProgExec() {
  for (char i = 0, i < 4, i++) {
    currentCommand[i] = 0;
  }
  commandIndex = 0;
  acc = 0;
}

char ProgExec::execute(char command) {

}