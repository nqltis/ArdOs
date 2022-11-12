#include <Arduino.h>
#include "M16input.h"
#include "LCDoutput.h"
#include "ustrlib.h"
#include "CAOfilesys.h"
#include "T9typelib.h"

byte InputPin[8] = {
  0, 1, A5, A4, A3, A2, A1, A0
};
M16input m16input(InputPin);

byte RS = 13;   //Register Select: H:data L:Instruction 
byte RW = 12;   //H:Read L:Write
byte E = 11;    //Enable Signal
byte DataPin[8] = {
  8, 9, 2, 3, 4, 5, 6, 7
};
LCDoutput lcdoutput(RS, RW, E, DataPin);

Ustrlib ustrlib;
CAOfilesys caofilesys;

int wdpath[16];   //working directory path (addresses)
byte level = 1;   //active wdpath[] index
int history[32];  //working dir file names
byte histPtr = 0; //active history[] index

char buff1[16] = "";
char buff2[16] = "";
void setup() {  
  history[0] = caofilesys.getContStart(0);
  wdpath[1] = 0;
  lcdoutput.init();
  printCurrent();
}

void printCurrent() {
  if (!histPtr) {
    buildPath(buff1);
    caofilesys.readFileName(buff2, history[histPtr]);
    lcdoutput.printScreen(buff1, buff2);
    if (caofilesys.isDir(history[histPtr])) lcdoutput.drawchar('>', 31);
    return;
  }
  caofilesys.readFileName(buff1, history[histPtr - 1]);
  caofilesys.readFileName(buff2, history[histPtr]);
  lcdoutput.printScreen(buff1, buff2);
  if (caofilesys.isDir(history[histPtr - 1])) lcdoutput.drawchar('>', 15);
  if (caofilesys.isDir(history[histPtr])) lcdoutput.drawchar('>', 31);
}
void selectPrevFile() {
  if (histPtr) histPtr--;
}
void selectNextFile() {
  int nextfile = caofilesys.nextFile(history[histPtr], wdpath[level]);
  if (!nextfile) return;
  history[histPtr + 1] = nextfile;
  histPtr++;
}
void selectPrevDir() {
  if (level <= 1) return;
  history[0] = caofilesys.getContStart(wdpath[level - 2]);
  histPtr = 0;
  level--;
  return;
}
void selectNextDir() {
  if (!caofilesys.isDir(history[histPtr])) return; //if not dir, return
  int content = caofilesys.getContStart(history[histPtr]);
  if (!content) {lcdoutput.drawchar('E', 30); delay(1000); return;}; //if dir empty, return
  level++;
  wdpath[level] = history[histPtr];   //save selected dir address
  histPtr = 0;
  history[0] = content;
}
void buildPath(char *output) {
  if (!level) {output[0] = 0; return;};
  output[0] = '/';
  output[1] = 0;
  for (int i = 1; i < level; i++) {
    char tmp[32];
    caofilesys.readFileName(tmp, wdpath[i+1]);
    ustrlib.strConcat(output, tmp);
    ustrlib.strConcat(output, "/");
  }
  return;
}

void loop() {
  switch (m16input.button()) {    //File Browser
    case 'A':
      selectPrevFile();
      printCurrent();
    break;
    case 'B':
      selectNextFile();
      printCurrent();
    break;
    case 'C':
      selectPrevDir();
      printCurrent();
    break;
    case 'D':
      selectNextDir();
      printCurrent();
    break;
    case '#':                //New file menu    
    {
      delay(10);
      TypeSession typesession;      //Create new input session
      char key = m16input.button(); //reset input key
      lcdoutput.printScreen("New file name:", "");
      while (key != '#') {    //press '#' to exit
        switch (key) {
          case 0: //no input : do nothing
          break;
          case 'A':
            typesession.eraseChar();
            lcdoutput.printScreen("New file name:", typesession.inputStr);
          break;
          case 'B':
            typesession.nextChar();
          break;
          case 'C':
          break;
          case 'D':
          break;
          case '*':
          break;
          default:
            typesession.enterKey(key);
            lcdoutput.printScreen("New file name:", typesession.inputStr);
          break;
        }
        key = m16input.button();
        delay(10);
      }
      lcdoutput.printScreen("Creating file:", typesession.inputStr);
      delay(2000);
      printCurrent();
      break;
    }
  }
  delay(10);
}
