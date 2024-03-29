/*
  T9typelib.h : Library for string input 
  with a 9 buttons keypad like an old phone.
  nqltis - 05/11/2022
*/

#ifndef T9typelib
#define T9typelib

class TypeSession {
  public:
    TypeSession();
    void enterKey(char key);
    void eraseChar();
    void nextChar();
    char inputStr[17];
    void chgCase();
    void numLock();
  private:
    int _index;
    char _activeKey;
    char _cycleStep;
    char _numLock;
    int keyToNum(char key);
};

#endif