/*
  ProgExec.h : Library for 
  Program Execution on ArdOs
  nqltis - 23/12/2022
*/

#ifndef progexec
#define progexec

class ProgExec
{
  public:
    ProgExec();
    char execute(char command);
    //char stack; //used by both main and lib to communicate syscalls
  private:
    unsigned char progmem[128];
    char currentCommand[4];
    char commandIndex;
    char acc;
};

#endif