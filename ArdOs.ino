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
  0, 0, 0, 0, 0   ,0 ,0                       //120 >file - endofusr - endofroot - nthing
};
int wdpath[16];   //working directory path (addresses)
byte level = 0;   //active wdpath[] index
int history[32];  //working dir file names
byte histPtr = 0; //active history[] index

//{blg{}, LO.txt(), a.txt(hello), usr{a.lua(!#lua5.3), main.c(int main() {})}}

int skipHeader(int address) {
  return address + (memory[address] & 127) + 3;
}
int readInt(int address) {
  return ((memory[address] << 8) | memory[address + 1]);
}
int isDir(int address) {
  return memory[address] & 128;
}

void _strConcat(char *output, char *str2) {
  int i = 0;
  int j = 0;
  while(output[j]) {
    j++;
  }
  i = 0;
  while(str2[i]) {
    output[j] = str2[i];
    i++;
    j++;
  }
  output[j] = 0;
  return;
}
int _strCompare(char *str1, char *str2) {
  int i = 0;
  while(str1[i] == str2[i]) {
    if (!(str1[i] || str2[i])) return 1;  //if both char are null, return true
    i++;
  }
  return 0;
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
int findAddr(char *fileName, int dirAddr) { //find memory address of a file in a given directory
                                            //return 0 if not found
  int address = dirAddr;
  address = skipHeader(address); //skip file header
  address = getContStart(address); //get to content
  if (!address) return 0; //if null, dir is empty
  while (readInt(address)) {
    char tempstr[16]; 
    readFileName(tempstr, address); //read name
    if (_strCompare(tempstr, fileName)) return address;
    address = nextFile(address); //go to next file
  }
  return 0; //end of dir
}
int getContStart(int address) { //follow redirections until content reached
                                //if end of file found before any content, return 0
  int start = readInt(address); //follow first redirection
  int end = readInt(start);
  while(1) { //while no null redirect :
    if (!(start && end)) return 0; //return 0 if end of file
    if ((start + 2) != end) return (start + 2); //return address if block not empty
    start = readInt(end);
    end = readInt(start); 
  }
  return 0; //is never reached
}

char buff1[16] = "";
char buff2[16] = "";
void setup() {  
  history[0] = 0;
  wdpath[0] = 0;
  lcdoutput.init();
  printCurrent();
}

void printCurrent() {
  if (!histPtr) {
    buildPath(buff1);
    readFileName(buff2, history[histPtr]);
    lcdoutput.printScreen(buff1, buff2);
    if (isDir(history[histPtr])) lcdoutput.drawchar('>', 31);
    return;
  }
  readFileName(buff1, history[histPtr - 1]);
  readFileName(buff2, history[histPtr]);
  lcdoutput.printScreen(buff1, buff2);
  if (isDir(history[histPtr - 1])) lcdoutput.drawchar('>', 15);
  if (isDir(history[histPtr])) lcdoutput.drawchar('>', 31);
}
void selectPrevFile() {
  if (histPtr) histPtr--;
}
void selectNextFile() {
  if (!memory[nextFile(history[histPtr])]) return;
  history[histPtr + 1] = nextFile(history[histPtr]);
  histPtr++;
}
void selectPrevDir() {
  switch (level) {
    case 1:
      history[0] = 0;
    break;
    case 0:
      return;
    default:
      history[0] = getContStart(skipHeader(wdpath[level - 2]));
  }
  histPtr = 0;
  level--;
  return;
}
void selectNextDir() {
  if (!isDir(history[histPtr])) return; //if not dir, return
  int content = skipHeader(history[histPtr]);
  content = getContStart(content);
  if (!content) {lcdoutput.drawchar('E', 30); delay(1000); return;}; //if dir empty, return
  level++;
  wdpath[level] = history[histPtr];   //save selected dir address
  histPtr = 0;
  history[0] = content;
}
void buildPath(char *output) {
  if (!level) {output[0] = 0; return;};
  output[0] = '/';
  output[1] = 0;
  for (int i = 1; i < level; i++) {
    char tmp[32];
    readFileName(tmp, wdpath[i+1]);
    _strConcat(output, tmp);
    _strConcat(output, "/");
  }
  return;
}

void loop() {
  switch (m16input.button()) {
    case 'A':
      selectPrevFile();
      printCurrent();
    break;
    case 'B':
      selectNextFile();
      printCurrent();
    break;
    case 'C':
      selectPrevDir();
      printCurrent();
    break;
    case 'D':
      selectNextDir();
      printCurrent();
    break;
  }
  delay(10);
}
