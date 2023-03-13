/*
  Filesysv3.h : Library for
  Custom ArdOs File System
  Version 3
  nqltis - 31/12/2022
*/

#ifndef CAO_FILE_SYSTEM_V3
#define CAO_FILE_SYSTEM_V3

#include <EEPROM.h>
#include "ustrlib.h"
#define BLOCK_SIZE 16
#define BLOCK_ID_TYPE unsigned char
#define BLOCK_ID_SIZE 1   //Number of bytes of BLOCK_ID_TYPE
#define INDEX_TYPE char
#define BLOCK_MAP_OFFSET 121  //Address of block map (20 file headers)
#define BLOCK_AREA_OFFSET 128  //Address of first data block (56 blocks)
#define MEMORY_SIZE 1024
#define HEADER_SIZE 6
#define HEADER_ID_TYPE unsigned char 
#define HEADER_ID_SIZE 1
#define ABS_ADDR_TYPE unsigned int//Size of absolute address (1024B -> int)

class File
{
  public:
    File();
    File(HEADER_ID_TYPE _headerPtr);
    void initfs(int offset);
    char isValid();
    char isDir();
    char isExecutable();
    void getName(char *output);
    File getParent();
    File getChild();
    File getPrev();
    File getNext();
    void open();
    char dataRemaining();
    //unsigned char read();
    void pathString(char *output);
    void mkfile(File workingDir, char *name);
    void mkdir(File workingDir, char *name);
    void mkexe(File workingDir, char *name);
    unsigned char read();
    void write(unsigned char data);
    unsigned char readRawMem(unsigned int pos);//debug
  private:
    HEADER_ID_TYPE headerPtr;
    BLOCK_ID_TYPE block;  //Set when open
    unsigned char index;  //Set when open
    HEADER_ID_TYPE getHeader();
    BLOCK_ID_TYPE getFirstBlock();
    BLOCK_ID_TYPE getFreeBlock();
    HEADER_ID_TYPE getFreeHeader();
    void reserveBlock(BLOCK_ID_TYPE blockId);
    void freeBlock(BLOCK_ID_TYPE blockId);
    void makefile(File workingDir, char *name, unsigned char flags);
};

#endif