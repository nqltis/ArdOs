/*
  Filesysv3.h : Library for
  Custom ArdOs File System
  Version 3
  nqltis - 31/12/2022
*/

#include <EEPROM.h>
#include "Filesysv3.h"

File::File() {  //Create Invalid File
  headerPtr = 255;
}

File::File(HEADER_ID_TYPE _headerPtr) {
  headerPtr = _headerPtr;
}

void File::initfs(int offset) {
  ABS_ADDR_TYPE _blockAddr = getFreeBlock();
  unsigned char rootBlock[4] = {0, 0, '/', 0};  //Write file name in block
  for (char i = 0; i < 4; i++) {
    EEPROM.update(_blockAddr + i, rootBlock[i]);
  }
  unsigned char rootHeader[5] = {0, 0, 0, 0, 0}; //Create header for root dir
  rootHeader[0] = _blockAddr;
  for (char i = 0; i < 5; i++) EEPROM.update(offset + i, rootHeader[i]);
}

char File::isDir() {
  return EEPROM.read(headerPtr) & 128;
}
char File::isExecutable() {
  return EEPROM.read(headerPtr) & 64;
}

void File::getName(char *output) {
  unsigned int offset = getFirstBlock() + 2*BLOCK_ID_SIZE;
  for (unsigned char i = 0; i < BLOCK_SIZE - 2*BLOCK_ID_SIZE; i++) {
    output[i] = EEPROM.read(i + offset);
    if (output[i] == 0) break;
  }
}

void File::getParent() {
  unsigned char ptrOffset = BLOCK_ID_SIZE;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr + ptrOffset);
  if (targetAddr != 0) headerPtr = targetAddr;
}
void File::getChild() {
  unsigned char ptrOffset = BLOCK_ID_SIZE + HEADER_ID_SIZE;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr + ptrOffset);
  if (targetAddr != 0) headerPtr = targetAddr;
}
void File::getPrev() {
  unsigned char ptrOffset = BLOCK_ID_SIZE + 2*HEADER_ID_SIZE;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr + ptrOffset);
  if (targetAddr != 0) headerPtr = targetAddr;
}
void File::getNext() {
  unsigned char ptrOffset = BLOCK_ID_SIZE + 3*HEADER_ID_SIZE;
  HEADER_ID_TYPE targetAddr = EEPROM.read(headerPtr + ptrOffset);
  if (targetAddr != 0) headerPtr = targetAddr;
}

BLOCK_ID_TYPE File::getFirstBlock() {
  return BLOCK_AREA_OFFSET + BLOCK_SIZE * EEPROM.read(headerPtr);
}
BLOCK_ID_TYPE File::getFreeBlock() {
  for (ABS_ADDR_TYPE i = 0; i < BLOCK_AREA_OFFSET - BLOCK_MAP_OFFSET; i++) {
    unsigned char _byte = EEPROM.read(i + BLOCK_MAP_OFFSET);
    for (char j = 0; j < 8; j++) {
      if (!(_byte & (1 << j))) return i * 8 + j;
    }
  } 
}
void File::reserveBlock(BLOCK_ID_TYPE blockId) {
  ABS_ADDR_TYPE targetByteAddr = blockId/8 + BLOCK_MAP_OFFSET;
  unsigned char targetByte = EEPROM.read(targetByteAddr);
  targetByte = targetByte | (1 << (blockId % 8));
}
void File::freeBlock(BLOCK_ID_TYPE blockId) {
  ABS_ADDR_TYPE targetByteAddr = blockId/8 + BLOCK_MAP_OFFSET;
  unsigned char targetByte = EEPROM.read(targetByteAddr);
  targetByte = targetByte & ~(1 << (blockId % 8));
}