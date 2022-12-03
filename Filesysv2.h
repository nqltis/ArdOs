/*
  caofilesys.h : Library for
  Custom ArdOs File System
  Version 2
  nqltis - 02/11/2022
*/

#ifndef CAO_FILE_SYSTEM_V2
#define CAO_FILE_SYSTEM_V2

#include "ustrlib.h"
#define FS_SIZE int //Size of pointers (limits memory size)

class File
{
  public:
    File(FS_SIZE address, File *parentDir);
    File(char *path); //get to file from path
    File(FS_SIZE address); //Create parentless file (root)
    void readFileName(char *str, int address);  //input char[] and file address. 
    int isDir(int address);   //Check if given file is a directory
    int isValid(int address); //Check if the file built from a char or address does actually exist
    int nextFile(int address, int dirAddr);
    int getContStart(int address); //follow redirections until content reached
    void getPathString(char *output);
    void enterDir();
    void exitDir();
    void selectNextFile(); 
  private:
    FS_SIZE path[16];
    char level;
    int findAddr(char *fileName, int dirAddr);
    int skipHeader(int address);
    int readInt(int address);
    int endDir(int address);
    int readNameSize(int address);
};



#endif