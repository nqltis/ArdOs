/*
  Filesysv3.h : Library for
  Custom ArdOs File System
  Version 3
  nqltis - 31/12/2022
*/

#ifndef CAO_FILE_SYSTEM_V3
#define CAO_FILE_SYSTEM_V3

//#include <EEPROM.h>

#define BLOCK_SIZE 16
#define BLOCK_ID_TYPE unsigned char
#define BLOCK_ID_SIZE 1   //Number of bytes of BLOCK_ID_TYPE
#define BLOCK_MAP_OFFSET 160  //Address of block map
#define BLOCK_AREA_OFFSET 224  //Address of first data block
#define HEADER_ID_TYPE unsigned char 
#define HEADER_ID_SIZE 1

class File
{
  public:
    File();
    File(HEADER_ID_TYPE _headerPtr);
    void initfs();
    char isDir();
    char isExecutable();
    void getName(char *output);
    void getNext();
    void getPrev();
    void getChild();
    void getParent();
    void open();
    char dataRemaining();
    unsigned char read();
  private:
    unsigned char headerPtr;
    unsigned char block;  //Set when open
    unsigned char index;  //Set when open
};

#endif