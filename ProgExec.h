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
    char getCommand();
    int getArg(char offset, unsigned char size);
    //char stack; //used by both main and lib to communicate syscalls
  private:
    char progmem[128];
    char arg[4];
    char argIndex;
    char commandLen;
    char acc;
};

#endif