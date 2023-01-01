/*
  Filesysv3.h : Library for
  Custom ArdOs File System
  Version 3
  nqltis - 31/12/2022
*/

#ifndef CAO_FILE_SYSTEM_V3
#define CAO_FILE_SYSTEM_V3

#include <EEPROM.h>

#define BLOCK_SIZE 16
#define BLOCK_ID_TYPE unsigned char
#define BLOCK_ID_SIZE 1   //Number of bytes of BLOCK_ID_TYPE
#define INDEX_TYPE char
#define BLOCK_MAP_OFFSET 160  //Address of block map
#define BLOCK_AREA_OFFSET 224  //Address of first data block
#define MEMORY_SIZE 1024
#define HEADER_ID_TYPE unsigned char 
#define HEADER_ID_SIZE 1
#define ABS_ADDR_TYPE unsigned int//Size of absolute address (1024B -> int)

class File
{
  public:
    File();
    File(HEADER_ID_TYPE _headerPtr);
    void initfs(int offset);
    char isDir();
    char isExecutable();
    void getName(char *output);
    void getParent();
    void getChild();
    void getPrev();
    void getNext();
    void open();
    char dataRemaining();
    unsigned char read();
  private:
    HEADER_ID_TYPE headerPtr;
    BLOCK_ID_TYPE block;  //Set when open
    unsigned char index;  //Set when open
    BLOCK_ID_TYPE getFirstBlock();
    BLOCK_ID_TYPE getFreeBlock();
    void reserveBlock(BLOCK_ID_TYPE blockId);
    void freeBlock(BLOCK_ID_TYPE blockId);
};

#endif