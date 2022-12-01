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
    FS_SIZE address;
    File parentDir;

  private:
}

#endif