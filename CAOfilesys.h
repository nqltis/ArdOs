/*
  caofilesys.h : Library for
  Custom ArdOs File System
  nqltis - 02/11/2022
*/

#ifndef CAOfilesys_h
#define CAOfilesys_h

#include <Arduino.h>
#include "ustrlib.h"

class CAOfilesys
{
  public:
    CAOfilesys();
    int skipHeader(int address);
    int readInt(int address);
    int isDir(int address);   //Check if given file is a directory
    void readFileName(char *str, int address);  //input char[] and file address. 
    int nextFile(int address, int dirAddr);
    int findAddr(char *fileName, int dirAddr); //find memory address of a file in a given directory
    int getContStart(int address); //follow redirections until content reached
  private:
    int endDir(int address);
    int readNameSize(int address);
    Ustrlib caofs_ustrlib;
};


#endif