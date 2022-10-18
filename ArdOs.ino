#include <Arduino.h>
#include "M16input/M16input.h"
#include "LCDoutput/LCDoutput.h"

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


int mempos = 0;
char memory[128] = {
  1, '/', 4, 'h', 'o', 'm', 'e', 6, 's', 'y', 's', 't', 'e', 'm' 
};

String readFileName(int adress) {
  String str = "";
  int endOfName = adress + memory[adress];
  for (int i = adress + 1; i <= endOfName; i++) {
    str = str + memory[i]; 
  }
  return str;
}
int nextFile(int adress) {
  return adress + memory[adress] + 1;
}

String buff1 = "";
String buff2 = "";
void setup() {
  buff2 = readFileName(mempos);
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
