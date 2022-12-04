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
#define FS_DEPTH 8 //Maximum depth of file system (Max level of dir tree)

class File
{
  public:
    File(FS_SIZE address, File *parentDir);
    File(char *path); //get to file from path
    File(FS_SIZE address); //Create parentless file (root)
    File(); //Create empty File
    void getName(char *str, int address);  //input char[] and file address. 
    void getName(char *str);  //input char[] and file address. 
    int isDir();   //Check if given file is a directory
    int isValid(); //Check if the file built from a char or address does actually exist
    File getNextFile();
    int getContStart(); //follow redirections until content reached
    void getPathString(char *output);
    void getParentString(char *output);
    File getParentDir();
    File enterDir();
    File copyFile();
  private:
    FS_SIZE path[FS_DEPTH];
    char level;
    int findAddr(char *fileName, int dirAddr);
    int skipHeader(int address);
    int readInt(int address);
    int endDir();
    int getNameSize();
};



#endif