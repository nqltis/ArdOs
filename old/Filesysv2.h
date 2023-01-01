/*
  Filesysv2.h : Library for
  Custom ArdOs File System
  Version 2
  nqltis - 02/11/2022
*/

#ifndef CAO_FILE_SYSTEM_V2
#define CAO_FILE_SYSTEM_V2

#include "ustrlib.h"
#define FS_SIZE int //Size of pointers (limits memory size)
#define FS_SIZE_N 2 //Number of bytes of pointers TODO: implement FS_SIZE
#define FS_DEPTH 8 //Maximum depth of file system (Max level of dir tree)

class File
{
  public:
    File(FS_SIZE address); //Create parentless file (root)
    File(); //Create empty File
    void getName(char *str);  //input char[] and file address. 
    char isDir();   //Check if file is a directory
    char isExecutable();  //Check if file is executable
    char isValid(); //Check if the file built from a char or address does actually exist
    File getNextFile();
    void getPathString(char *output);
    File getParentDir();
    File enterDir();
    File copyFile();
    char open();
    char dataRemaining();
    unsigned char readData();
    void initRoot();
  private:
    FS_SIZE path[FS_DEPTH];
    char level;
    FS_SIZE startOfBlock;
    FS_SIZE endOfBlock;
    FS_SIZE currentAddress;
    int getContStart(); //follow redirections until content reached
    FS_SIZE followRedirect(FS_SIZE _initAddr); //getContStart without header skipping
    int skipHeader(int address);
    int readInt(int address);
    int endDir();
    unsigned char getNameSize();
};



#endif