/*
  caofilesys.h : Library for
  Custom ArdOs File System
  Version 2
  nqltis - 02/11/2022
*/

#include "ustrlib.h"
#include "Filesysv2.h"

#define fsz 0 //dummy for futur implementation of file size
#define empt 0 //dummy for empty mem

#define NOP 0
#define LD 1
#define ST 2
#define AND 3
#define OR 4
#define NOT 5
#define XOR 6
#define ADD 7
#define SUB 8
#define MUL 9
#define DIV 10
#define MOD 11
#define EQ 12
#define GT 13
#define LT 14
#define GE 15
#define LE 16
#define JMP 17
#define lpst 192
#define jmp 193
#define ext 255
#define slp 254
#define pch 253
#define pst 252

static const unsigned char memory[] = {
  129, '/', fsz, fsz, 0, 6, 0, 50,            //0   /{
  131, 'b', 'l', 'g', fsz, fsz, 0, 16,        //8   blg(
  0, 0, 6, 'L', 'O', '.', 't', 'x',           //16  ), syste
  't', fsz, fsz, 0, 29, 0, 0, 5,              //24  m{>  },
  'a', '.', 't', 'x', 't', fsz, fsz, 0,       //32  a.txt
  41, 0, 48, 'h', 'e', 'l', 'l', 'o',         //40  (hello
  0, 57, 0, 55, empt, empt, empt, 0,          //48
  129, 0, 0, 131, 'u', 's', 'r', fsz,         //56  
  fsz, 0, 69, empt, empt, 0, 125, 5,          //64
  'm', 'y', 'l', 'u', 'a', fsz, fsz, 0,       //72
  81, 0, 91, '!', '#', 'l', 'u', 'a',         //80
  '5', '.', '3', 0, 93, 0, 0, 6,              //88
  'm', 'a', 'i', 'n', '.', 'c', fsz, fsz,     //96
  0, 106, 0, 121, 'i', 'n', 't', ' ',         //104
  'm', 'a', 'i', 'n', '(' ,')', ' ', '{',     //112
  '}', 0, 137, empt, empt, 0, 135, empt,      //120
  empt, 0, 133, empt, empt, 0, 196, 0,        //128
  192, 0, 0, 3, 'e', 'x', 'e', fsz,           //136
  fsz, 0, 147, 0, 188, LD, 'A', ST,            //144
  20, LD, 0, ST, 21, lpst, pch, 148,         //152
  149, LD, 148, ADD, 1, ST, 20, JMP,           //160
  6, LD, 149, ADD, 1, ST, 21, slp,        //168
  1, 0, jmp, ext, empt, empt, empt, empt,  //176
  empt, empt, empt, ext, 0, 190, 0, 0,           //184
  0, 194, 0, 0, 0, 198, 0, 0                      //192
};
/* empty exe file
  fsz, 0, 147, 0, 188, empt, empt, empt,            //144
  empt, empt, empt, empt, empt, empt, empt, empt,         //152
  empt, empt, empt, empt, empt, empt, empt, empt,           //160
  empt, empt, empt, empt, empt, empt, empt, empt,        //168
  empt, empt, empt, empt, empt, empt, empt, empt, //176
  empt, empt, empt, empt, 0, 190, 0, 0,           //184
  0, 194, 0, 0, 0, 198, 0, 0                      //192
*/
//{blg{}, LO.txt(), a.txt(hello), usr{a.lua(!#lua5.3), main.c(int main() {}), exe}}

File::File(FS_SIZE _address) {
  path[0] = _address;
  level = 0;
}

File::File() {
  level = -127;
}

File File::copyFile() {
  File copy;
  for (char i = 0; i <= level; i++) {
    copy.path[i] = path[i];
  }
  copy.level = level;
  copy.startOfBlock = startOfBlock;
  copy.endOfBlock = endOfBlock;
  copy.currentAddress = currentAddress;
  return copy;
}

int File::skipHeader(int address) {
  return address + (memory[address] & 127) + 3;
}
int File::readInt(int address) {
  return ((memory[address] << 8) | memory[address + 1]);
}
int File::isDir() {
  return memory[path[level]] & 128;
}
int File::getNameSize() {
  return memory[path[level]] & 127;
}
int File::isValid() {
  if (level < 0) return 0; return 1;
}

void File::getName(char *str) {  //input char[] and file address. 
                                              //write in char[] name of file at address.
  int sizeOfName = getNameSize(); //get rid of dir flag
  for (int i = 1; i <= sizeOfName; i++) {
    str[i - 1] = memory[path[level] + i]; 
  }
  str[sizeOfName] = 0;
  return;
}

File File::getNextFile() {  //return next file address if not at end of working directory
  int address = path[level]; 
  int nextaddr = address + getNameSize() + 3;    //address of redirect
  do {  //follow redirections until endoffile
    address = nextaddr; 
    nextaddr = readInt(nextaddr); //read address of endofblock
  } while (nextaddr);
  address += 2; //go to first byte after
  File parentDir = File(path[level - 1]); 
  if (address == parentDir.endDir()) return File(); //end of directory
  File nextFile = copyFile();
  nextFile.path[nextFile.level] = address;
  return nextFile;
}

//Only used by nextfile() to detect end of working directory, dir must not be empty
int File::endDir() {  //return end address of parent dir last block
  int newHeaderPtr = readInt(path[level] + getNameSize() + 3);
  int newHeaderVal = readInt(newHeaderPtr);
  int eobPtr;
  while (newHeaderVal) { //follow redirections until endofdir
    eobPtr = newHeaderVal; // = old header ptr
    newHeaderPtr = readInt(eobPtr);
    newHeaderVal = readInt(newHeaderPtr);
  }
  return eobPtr;
}

int File::getContStart() { //skip header then follow redirections until content reached
                                //if end of file found before any content, return 0
  int start = readInt(skipHeader(path[level])); //skip header then follow first redirection
  int end = readInt(start);
  while(1) { //while no null redirect :
    if (!(start && end)) return 0; //return 0 if end of file
    if ((start + 2) != end) return (start + 2); //return address if block not empty
    start = readInt(end);
    end = readInt(start); 
  }
  return 0; //is never reached
}

FS_SIZE File::followRedirect(FS_SIZE _initAddr) { //skip header then follow redirections until content reached
                                //if end of file found before any content, return 0
  int start = readInt(_initAddr); //skip header then follow first redirection
  int end = readInt(start);
  while(1) { //while no null redirect :
    if (!(start && end)) return 0; //return 0 if end of file
    if ((start + 2) != end) return (start + 2); //return address if block not empty
    start = readInt(end);
    end = readInt(start); 
  }
  return 0; //is never reached
}

void File::getPathString(char *output) {
  output[0] = '/';
  output[1] = 0;
  for (int i = 1; i <= level; i++) {
    File temp = File(path[i]);
    char tmp[32];
    temp.getName(tmp);
    strConcat(output, tmp);
    strConcat(output, "/");
  }
  return;
}

File File::getParentDir() {
  if (level <= 0) return File();
  File parent;
  for (char i = 0; i < level; i++) {
    parent.path[i] = path[i];
  }
  parent.level = level - 1;
  return parent;
}

File File::enterDir() {
  int nextFile = getContStart();
  if (nextFile) {
    File copy = copyFile();
    copy.level++;
    copy.path[copy.level] = nextFile;
    return copy;
  } else {
    File nullfile = File();
    return nullfile;
  }
}

char File::open() {
  startOfBlock = getContStart();
  endOfBlock = readInt(startOfBlock);
  currentAddress = startOfBlock;
  if (startOfBlock) return 1; //success
  return 0; //empty file
}

char File::dataRemaining() {
  return startOfBlock;
}

unsigned char File::readData() {
  if (currentAddress == endOfBlock) {
    startOfBlock = followRedirect(startOfBlock);
    endOfBlock = readInt(startOfBlock);
    currentAddress = startOfBlock + 2;
  }
  return memory[currentAddress++];
}