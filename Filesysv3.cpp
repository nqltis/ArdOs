/*
  Filesysv3.h : Library for
  Custom ArdOs File System
  Version 3
  nqltis - 31/12/2022
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "ustrlib.h"
#include "Filesysv3.h"

File::File() {  //Create Invalid File
  headerPtr = 255;
}

File::File(HEADER_ID_TYPE _headerPtr) {
  headerPtr = _headerPtr;
}

void File::initfs(int offset) {
  for (unsigned int i = 0; i < MEMORY_SIZE; i++) {
    EEPROM.update(i, 0);
  }
  ABS_ADDR_TYPE _blockAddr = BLOCK_AREA_OFFSET;
  reserveBlock(0);
  unsigned char rootBlock[4] = {0, 0, '/', 0};  //Write file name in block
  for (char i = 0; i < 4; i++) {
    EEPROM.update(_blockAddr + i, rootBlock[i]);
  }
  for (unsigned int i = 0; i < BLOCK_MAP_OFFSET; i += 6) {
    EEPROM.update(i, 0);  //Write 0 at every header start
  }
  unsigned char rootHeader[6] = {192, 0, 0, 0, 0, 0}; //Create header for root dir
  for (char i = 0; i < 6; i++) EEPROM.update(offset + i, rootHeader[i]);
}

char File::isValid() {
  return (headerPtr != 255);
}
char File::isDir() {
  return (EEPROM.read(headerPtr * HEADER_SIZE) & 64) && isValid();
}
char File::isExecutable() {
  return (EEPROM.read(headerPtr * HEADER_SIZE) & 32) && isValid();
}

void File::getName(char *output) {
  unsigned int offset = BLOCK_AREA_OFFSET + BLOCK_SIZE * getFirstBlock() + 2*BLOCK_ID_SIZE;
  for (unsigned char i = 0; i < BLOCK_SIZE - 2*BLOCK_ID_SIZE; i++) {
    output[i] = EEPROM.read(i + offset);
    if (output[i] == 0) break;
  }
}

File File::getParent() {
  if (!isValid()) return File();
  unsigned char ptrOffset = BLOCK_ID_SIZE + 1;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr * HEADER_SIZE + ptrOffset);
  return File(targetAddr);
}
File File::getChild() {
  if (!isValid()) return File();
  unsigned char ptrOffset = BLOCK_ID_SIZE + HEADER_ID_SIZE + 1;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr * HEADER_SIZE + ptrOffset);
  if (targetAddr != 0) return File(targetAddr);
  return File();
}
File File::getPrev() {
  if (!isValid()) return File();
  unsigned char ptrOffset = BLOCK_ID_SIZE + 2*HEADER_ID_SIZE + 1;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr * HEADER_SIZE + ptrOffset);
  if (targetAddr != 0) return File(targetAddr);
  return File();
}
File File::getNext() {
  if (!isValid()) return File();
  unsigned char ptrOffset = BLOCK_ID_SIZE + 3*HEADER_ID_SIZE + 1;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr * HEADER_SIZE + ptrOffset);
  if (targetAddr != 0) return File(targetAddr);
  return File();
}

void File::open() {
  if (isDir()) return;
  block = getFirstBlock();//init
  char i;
  for (i = 2*BLOCK_ID_SIZE; i < BLOCK_SIZE; i++) {  //skip file name
    if (!EEPROM.read(block * BLOCK_SIZE + i + BLOCK_AREA_OFFSET)) {i++; break;}
  }
  index = i;
  if (index == BLOCK_SIZE - 2*BLOCK_ID_SIZE - 1) {  //if at end of block
    block = EEPROM.read(block * BLOCK_SIZE + BLOCK_ID_SIZE + BLOCK_AREA_OFFSET); //Jump to next block
    index = 2*BLOCK_ID_SIZE;
  }
  return;
}

char File::dataRemaining() {  //Return True if still in a block containing data
  return block;
}

unsigned char File::read() {  //Read next byte of open file
  index++;
  if (index == BLOCK_SIZE) {  //if at end of block
    block = EEPROM.read(block * BLOCK_SIZE + BLOCK_ID_SIZE + BLOCK_AREA_OFFSET); //Jump to next block
    index = 2*BLOCK_ID_SIZE;
  }
  if (dataRemaining()) return EEPROM.read(index);
  else return 0;
}

void File::pathString(char *output) { //Recursive function to get path of given file
  char str[BLOCK_SIZE - 1];
  getName(str);
  if (((str[0] == '/') && (str[1] == 0)) || !isValid()) { //init
    output[0] = '/';
    output[1] = 0;
    return;
  }
  File parent = getParent();
  parent.pathString(output);
  strConcat(str, "/");
  strConcat(output, str);
  return;
}

void File::mkfile(File workingDir, char *name) {
  makefile(workingDir, name, 0);
}
void File::mkdir(File workingDir, char *name) {
  makefile(workingDir, name, 64);
}
void File::mkexe(File workingDir, char *name) {
  makefile(workingDir, name, 32);
}

BLOCK_ID_TYPE File::getFirstBlock() { //Get first data block of given file
  return (EEPROM.read(headerPtr * HEADER_SIZE + 1));
}
BLOCK_ID_TYPE File::getFreeBlock() {  //Get unused data block ID
  for (ABS_ADDR_TYPE i = 0; i < BLOCK_AREA_OFFSET - BLOCK_MAP_OFFSET; i++) {
    unsigned char _byte = EEPROM.read(i + BLOCK_MAP_OFFSET);
    for (char j = 0; j < 8; j++) {
      if (!(_byte & (1 << j))) return i * 8 + j;
    }
  } 
}
HEADER_ID_TYPE File::getFreeHeader() {  //Get unused file header ID
  for (HEADER_ID_TYPE i = 0; i < (BLOCK_MAP_OFFSET/HEADER_SIZE); i++) {
    if (!(EEPROM.read(i * HEADER_SIZE) & 128)) return i;
  }
  return 255; //Memory full
}
void File::reserveBlock(BLOCK_ID_TYPE blockId) {  //Mark block as owned by an existing file
  ABS_ADDR_TYPE targetByteAddr = blockId/8 + BLOCK_MAP_OFFSET;
  unsigned char targetByte = EEPROM.read(targetByteAddr);
  targetByte = targetByte | (1 << (blockId % 8));
  EEPROM.update(targetByteAddr, targetByte);
}
void File::freeBlock(BLOCK_ID_TYPE blockId) { //Mark block as unused
  ABS_ADDR_TYPE targetByteAddr = blockId/8 + BLOCK_MAP_OFFSET;
  unsigned char targetByte = EEPROM.read(targetByteAddr);
  targetByte = targetByte & ~(1 << (blockId % 8));
  EEPROM.update(targetByteAddr, targetByte);
}
void File::makefile(File workingDir, char *name, unsigned char flags) { //Generic file creator
  BLOCK_ID_TYPE newBlock = getFreeBlock();  //Find free block
  reserveBlock(newBlock);
  ABS_ADDR_TYPE blockOffset = newBlock * BLOCK_SIZE + BLOCK_AREA_OFFSET;
  EEPROM.update(blockOffset++, 0);
  EEPROM.update(blockOffset++, 0);
  for (char i = 0; i < BLOCK_SIZE - 2*BLOCK_ID_SIZE; i++) { //Write name to block
    EEPROM.update(blockOffset + i, name[i]);
    if (!name[i]) break;
  }
  HEADER_ID_TYPE newHeader = getFreeHeader(); //Find free header
  unsigned char fileHeader[6];
  fileHeader[0] = 128 | flags;  //flags
  fileHeader[1] = newBlock;     //first data block ID
  fileHeader[2] = workingDir.headerPtr; //Parent dir header ID
  fileHeader[3] = 0;                    //Child file header ID
  File _file = workingDir.getChild();
  if (!_file.isValid()) {
    EEPROM.update(workingDir.headerPtr * HEADER_SIZE + BLOCK_ID_SIZE + HEADER_ID_SIZE + 1, newHeader);
    fileHeader[4] = 0;
  } else {
    while (_file.getNext().isValid()) { //Get last file of working dir
      _file = _file.getNext();
    }
    EEPROM.update(_file.headerPtr * HEADER_SIZE + BLOCK_ID_SIZE + 3*HEADER_ID_SIZE + 1, newHeader); //Set as next file of found file
    fileHeader[4] = _file.headerPtr;    //Previous file header ID
  }
  fileHeader[5] = 0;                    //Next file header ID
  for (char i = 0; i < 6; i++) {  //Copy prepared file header to memory
    EEPROM.update(newHeader * HEADER_SIZE + i, fileHeader[i]);
  }
}

unsigned char File::readRawMem(unsigned int pos) {  //Debug : Read byte of memory at given address
  return EEPROM.read(pos);
}
