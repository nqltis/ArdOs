#include <Arduino.h>
#include <M16input.h>

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

void pulse() {
  delay(1);
  digitalWrite(E, HIGH);
  delay(1);
  digitalWrite(E, LOW);
  delay(1);
}
void dwrite(byte data){
  for (byte i = 0; i < 8; i++) {
    digitalWrite(DataPin[i], !!(data & (1 << i)));
  }
}
void execinstr(byte data) {
  digitalWrite(RS, LOW);
  dwrite(data);
  pulse();
}
void senddata(byte data) {
  digitalWrite(RS, HIGH);
  dwrite(data);
  pulse();
}
void drawchar(char chr, int pos) {
  if (pos >= 16) pos+= 48;
  execinstr(128 | pos);
  senddata(chr);
}
void printScreen(String line1, String line2) {
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
  pinMode(RS, OUTPUT);
  pinMode(RW, OUTPUT);
  pinMode(E, OUTPUT);
  digitalWrite(RS, LOW);
  digitalWrite(RW, LOW);
  digitalWrite(E, LOW);
  for (byte i = 0; i < 8; i++) {
    pinMode(DataPin[i], OUTPUT);
    digitalWrite(DataPin[i], LOW);
  }
  execinstr(1);    //Clear the screen
  execinstr(2);    //Return home
  execinstr(6);    //Entry mode
  execinstr(14);   //Display on, Cursor on
  execinstr(20);   //Enable autoshift right
  execinstr(58);   //Function set
  execinstr(12);   //Cursor off
  buff2 = readFileName(mempos);
  printScreen(buff1, buff2);
}

void loop() {
  switch (m16input.button()) {
    case 'B':{
      mempos = nextFile(mempos);
      if (memory[mempos]) {
        buff1 = buff2;
        buff2 = readFileName(mempos);
        printScreen(buff1, buff2);
      }
    break;
    }
  }
}
