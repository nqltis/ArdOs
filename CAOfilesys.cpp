/*
  caofilesys.h : Library for
  Custom ArdOs File System
  nqltis - 02/11/2022
*/

#include "ustrlib.h"
#include "CAOfilesys.h"

#define fsz 0 //dummy for futur implementation of file size
#define empt 0 //dummy for empty mem

CAOfilesys::CAOfilesys() {}

unsigned char memory[] = {
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
  '}', 0, 123, 0, 0, 0, 127, 0,               //120
  0, 0, 131, 0, 0                             //128
};

//{blg{}, LO.txt(), a.txt(hello), usr{a.lua(!#lua5.3), main.c(int main() {})}}

int CAOfilesys::skipHeader(int address) {
  return address + (memory[address] & 127) + 3;
}
int CAOfilesys::readInt(int address) {
  return ((memory[address] << 8) | memory[address + 1]);
}
int CAOfilesys::isDir(int address) {
  return memory[address] & 128;
}
int CAOfilesys::readNameSize(int address) {
  return memory[address] & 127;
}

void CAOfilesys::readFileName(char *str, int address) {  //input char[] and file address. 
                                              //write in char[] name of file at address.
  int sizeOfName = readNameSize(address); //get rid of dir flag
  for (int i = 1; i <= sizeOfName; i++) {
    str[i - 1] = memory[address + i]; 
  }
  str[sizeOfName] = 0;
  return;
}

int CAOfilesys::nextFile(int address, int parentDirAddr) {  //return next file address if not at end of working directory
  int nsize = readNameSize(address); //get rid of dir flag 
  int nextaddr = address + nsize + 3;    //address of redirect
  do {  //follow redirections until endoffile
    address = nextaddr; 
    nextaddr = readInt(nextaddr); //read address of endofblock
  } while (nextaddr);
  address += 2; //go to first byte after 
  if (address == endDir(parentDirAddr)) return 0; //end of directory
  return (address);
}

//Only used by nextfile() to detect end of working directory, dir must not be empty
int CAOfilesys::endDir(int address) {  //return end address of working dir last block
  int nsize = readNameSize(address);        //get rid of dir flag 
  int newHeaderPtr = readInt(address + nsize + 3);
  int newHeaderVal = readInt(newHeaderPtr);
  int eobPtr;
  while (newHeaderVal) { //follow redirections until endofdir
    eobPtr = newHeaderVal; // = old header ptr
    newHeaderPtr = readInt(eobPtr);
    newHeaderVal = readInt(newHeaderPtr);
  }
  return eobPtr;
}

int CAOfilesys::findAddr(char *fileName, int dirAddr) { //find memory address of a file in a given directory
                                            //return 0 if not found
  int address = dirAddr;
  address = skipHeader(address); //skip file header
  address = getContStart(address); //get to content
  if (!address) return 0; //if null, dir is empty
  while (readInt(address)) {  //TODO : remove readInt()
    char tempstr[16]; 
    readFileName(tempstr, address); //read name
    if (strCompare(tempstr, fileName)) return address;
    address = nextFile(address, dirAddr); //go to next file
  }
  return 0; //end of dir
}

int CAOfilesys::getContStart(int address) { //skip header then follow redirections until content reached
                                //if end of file found before any content, return 0
  int start = readInt(skipHeader(address)); //skip header then follow first redirection
  int end = readInt(start);
  while(1) { //while no null redirect :
    if (!(start && end)) return 0; //return 0 if end of file
    if ((start + 2) != end) return (start + 2); //return address if block not empty
    start = readInt(end);
    end = readInt(start); 
  }
  return 0; //is never reached
}