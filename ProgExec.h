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
    char ignore(char command);
    char getCommand();
    int getArg(char offset, unsigned char size);
    void getProgMem(char *output, unsigned char offset, unsigned char size);
  private:
    char progmem[128];
    char arg[4];
    char argIndex;
    char commandLen;
    char acc;
    char cond;
    void setCommandLen(char command);
};

#endif