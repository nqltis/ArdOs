#include <Arduino.h>
#include "M16input.h"
#include "LCDoutput.h"

byte InputPin[8] = {
  0, 1, A5, A4, A3, A2, A1, A0
};
M16input m16input(InputPin);

byte RS = 13;   //Register Select: H:data L:Instruction 
byte RW = 12;   //H:Read L:Write
byte E = 11;    //Enable Signal
byte DataPin[8] = {
  8, 9, 2, 3, 4, 5, 6, 7
};
LCDoutput lcdoutput(RS, RW, E, DataPin);


#define fsz 0 //dummy for futur implementation of file size

int mempos = 0;
char memory[128] = {
  1, '/', fsz, fsz, 0, 23, 4, 'h', 
  'o', 'm', 'e', fsz, fsz, 0, 10, 0, 
  6, 's', 'y', 's', 't', 'e', 'm', fsz, 
  fsz, 0, 20, 0, 0
};

//nsz, n,a,m,e, fsz, fsz, eob, eob, {ctnt...}, 0

String readFileName(int address) {
  String str = "";
  int endOfName = address + memory[address];
  for (int i = address + 1; i <= endOfName; i++) {
    str = str + memory[i]; 
  }
  return str;
}
int nextFile(int address) {
  int endOfBlock;
  int addr;
  do {
    addr = address + memory[address] + 3; // address of first byte of eob
    //        Start of +  Size of name + skip
    //          file                   file size
    endOfBlock = (memory[addr] << 8) | memory[addr + 1];
  } while (endOfBlock);
  return (addr + 2);
}

String buff1 = "";
String buff2 = "";
void setup() {
  buff2 = readFileName(mempos);
  lcdoutput.init();
  lcdoutput.printScreen(buff1, buff2);
}

void loop() {
  switch (m16input.button()) {
    case 'B':
      mempos = nextFile(mempos);
      if (memory[mempos]) {
        buff1 = buff2;
        buff2 = readFileName(mempos);
        lcdoutput.printScreen(buff1, buff2);
      }
    break;
  }
}
