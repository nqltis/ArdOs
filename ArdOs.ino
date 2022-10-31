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
#define empt 0 //dummy for empty mem

char memory[] ={
  129, '/', fsz, fsz, 0, 6, 0, 50,            //0   /{
  131, 'b', 'l', 'g', fsz, fsz, 0, 16,        //8   blg(
  0, 0, 6, 'L', 'O', '.', 't', 'x',           //16  ), syste
  't', fsz, fsz, 0, 29, 0, 0, 5,              //24  m{>  },
  'a', '.', 't', 'x', 't', fsz, fsz, 0,       //32  a.txt
  41, 0, 48, 'h', 'e', 'l', 'l', 'o',         //40  (hello
  0, 59, 0, 57,
                empt, empt, empt, empt,       //48   
  empt, 0, 123, 0, 0, 131, 'u', 's',          //56  
  'r', fsz, fsz, 0, 69, 0, 121, 5,            //64
  'm', 'y', 'l', 'u', 'a', fsz, fsz, 0,       //72
  81, 0, 91, '!', '#', 'l', 'u', 'a',         //80
  '5', '.', '3', 0, 0, 6, 'm', 'a',           //88
  'i', 'n', '.', 'c', fsz, fsz, 0, 104,       //96
  0, 120, 'i', 'n', 't', ' ', 'm', 'a',       //104
  'i', 'n', '(' ,')', ' ', '{', '}', 0,       //112 endof>
  0, 0, 0, 0, 0                               //120 >file - endofusr - endofroot
};
int history[32];
byte histPtr = 0;

//{blg{}, LO.txt(), a.txt(hello), usr{a.lua(!#lua5.3), main.c(int main() {})}}

//nsz + 128(dir), n,a,m,e, fsz, fsz, red, red
//eoChunkaddr (2B), {content}, red, red 
//eob, eob

void _strCopy(char *str1, char *str2) { //copy str1 to str2
  int i = 0;
  while (str1[i]) {
    str2[i] = str1[i];
    i++;
  }
  str2[i] = 0;
  return;
}
void readFileName(char *str, int address) {  //input char[] and file address. 
                                              //write in char[] name of file at address.
  int sizeOfName = memory[address] & 127; //get rid of dir flag
  for (int i = 1; i <= sizeOfName; i++) {
    str[i - 1] = memory[address + i]; 
  }
  str[sizeOfName] = 0;
  return;
}
int nextFile(int address) {
  int addr = 0;
  int nextaddr = address;
  int nsize = memory[nextaddr] & 127; //get rid of dir flag 
  nextaddr += nsize + 3;    //address of eob
  //  Start of +  Size of name + skip
  //    file                   file size
  do {
    addr = nextaddr;
    nextaddr = (memory[nextaddr] << 8) | memory[nextaddr + 1]; //read address of endofblock
  } while (nextaddr);
  return (addr + 2);
}

char buff1[16] = "";
char buff2[16] = "";
void setup() {  
  history[0] = 8;
  readFileName(buff2, history[histPtr]);
  lcdoutput.init();
  lcdoutput.printScreen(buff1, buff2);
}

void printCurrent() {
  if (!histPtr) {
    buff1[0] = 0;
    readFileName(buff2, history[histPtr]);
    lcdoutput.printScreen(buff1, buff2);
    return;
  }
  readFileName(buff1, history[histPtr - 1]);
  readFileName(buff2, history[histPtr]);
  lcdoutput.printScreen(buff1, buff2);
}
void selectPrev() {
  if (histPtr) histPtr--;
}
void selectNext() {
  if (!memory[nextFile(history[histPtr])]) return;
  history[histPtr + 1] = nextFile(history[histPtr]);
  histPtr++;
}

void loop() {
  switch (m16input.button()) {
    case 'B':
      selectNext();
      printCurrent();
    break;
    case 'A':
      selectPrev();
      printCurrent();
    break;
  }
  delay(10);
}
