#include <Arduino.h>
#include "M16input.h"
#include "LCDoutput.h"
#include "Filesysv2.h"
#include "ustrlib.h"
#include "T9typelib.h"
#include "ProgExec.h"

const byte InputPin[8] = {
  0, 1, A5, A4, A3, A2, A1, A0
};
M16input m16input(InputPin);

const byte RS = 13;   //Register Select: H:data L:Instruction 
const byte RW = 12;   //H:Read L:Write
const byte E = 11;    //Enable Signal
const byte DataPin[8] = {
  8, 9, 2, 3, 4, 5, 6, 7
};
LCDoutput lcdoutput(RS, RW, E, DataPin);

File selectedFile(0); //root
File shadowFile; //empty File

byte fileIndex = 0; //Selected File index in directory

char buff1[16] = "";
char buff2[16] = "";
void setup() {  
  lcdoutput.init();
  selectedFile.getName(buff1);  //TODO: unsafe if name size > 16
  //if ((buff1[0] != '/') || (buff1[1] != 0)) { //If first dir isn't root, it is invalid
    lcdoutput.printScreen(" Root not found", "Redirect to ctrl");
    delay(2000);
    lcdoutput.printScreen("==Control Menu==", "1:initFS");
    char inMenu = 1;
    while(inMenu) {
      switch (m16input.button()) {
        case 'C':
          inMenu = 0;
        break;
        case '1':
          selectedFile.initRoot();
          inMenu = 0;
        break;
      }
    }
  //}
  selectedFile = selectedFile.enterDir(); //TODO: check if root is valid
  printCurrent();
}

void printCurrent() {
  if (!fileIndex) {
    File parent = selectedFile.getParentDir();
    parent.getPathString(buff1);
    selectedFile.getName(buff2);
    lcdoutput.printScreen(buff1, buff2);
    if (selectedFile.isExecutable()) lcdoutput.drawchar('*', 31);
    if (selectedFile.isDir()) lcdoutput.drawchar('>', 31);
    return;
  }
  shadowFile.getName(buff1);
  selectedFile.getName(buff2);
  lcdoutput.printScreen(buff1, buff2);
  if (shadowFile.isExecutable()) lcdoutput.drawchar('*', 15);
  if (shadowFile.isDir()) lcdoutput.drawchar('>', 15);
  if (selectedFile.isExecutable()) lcdoutput.drawchar('*', 31);
  if (selectedFile.isDir()) lcdoutput.drawchar('>', 31);
}
void selectPrevFile() {
  selectedFile = selectedFile.getParentDir(); //Exit then enter dir
  selectedFile = selectedFile.enterDir();
  fileIndex = 0;
}
void selectNextFile() {
  File next = selectedFile.getNextFile();
  if (!next.isValid()) return;
  shadowFile = selectedFile;
  selectedFile = next;
  fileIndex++;
}
void selectPrevDir() {
  File parent = selectedFile.getParentDir();
  if (!parent.isValid()) return;
  selectedFile = selectedFile.getParentDir();
  fileIndex = 0;
  selectPrevFile();
}
void selectNextDir() {
  if (!selectedFile.isDir()) return; //if not dir, return
  File content = selectedFile.enterDir();
  if (!content.isValid()) {lcdoutput.drawchar('E', 30); delay(1000); return;}; //if dir empty, return
  selectedFile = content;
  fileIndex = 0;
}
void controlMenu() {
  lcdoutput.printScreen("1:mkf 2:mkd 3:rm", "4:dbg 5:chex");
  char inMenu = 1;
  while(inMenu) {
    switch (m16input.button()) {
      case 'C':
        inMenu = 0;
      break;
    }
  }
}

void stringInput(char *output, char *phrase) {
  TypeSession typesession;      //Create new input session
  char key = m16input.button(); //reset input key
  lcdoutput.printScreen(phrase, "");
  while (key != 'D') {    //press 'D' to exit
    switch (key) {
      case 0: //no input : do nothing
      break;
      case 'A':
        typesession.eraseChar();
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
      case 'B':
        typesession.nextChar();
      break;
      case 'C': //TODO : Cancel
      break;
      case '#': //Shift
        typesession.chgCase();
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
      case '*':
      break;
      default:
        typesession.enterKey(key);
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
    }
    key = m16input.button();
  }
  for (char i = 0; i < 16; i++) {
    output[i] = typesession.inputStr[i];
  }
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

    case '#':   //Control menu
      controlMenu();
      printCurrent();
    break;
    /*{ //New file menu
      char inputStr[16];
      stringInput(inputStr, "New file name :");
      lcdoutput.printScreen("Creating file:", inputStr);
      delay(2000);
      printCurrent();
    }
    break;  */

    case '*':{ //Execute file
      if (!selectedFile.isExecutable()) break;
      selectedFile.open();
      ProgExec thread;
      char running = 1;
      while(selectedFile.dataRemaining() && running) {
        char progbyte = selectedFile.readData();
        char callcode = thread.execute(progbyte);
        /* debug  
        char text[4];
        toString(text, progbyte);
        lcdoutput.drawchar(text[0], 28);
        lcdoutput.drawchar(text[1], 29);
        lcdoutput.drawchar(text[2], 30);
        lcdoutput.drawchar(text[3], 31);
        toString(text, callcode);
        lcdoutput.drawchar(text[0], 12);
        lcdoutput.drawchar(text[1], 13);
        lcdoutput.drawchar(text[2], 14);
        lcdoutput.drawchar(text[3], 15);
        delay(1000);
        // */
        if (callcode < 0) { //handle syscall
          switch (callcode) {
            case -1:  //ext
              running = 0; //end of program
            break;
            case -2:  //slp I/R I/R
              delay(thread.getArg(0, 2)); //waiting for two more arguments
            break;
            case -3: break; //lab I (no action on reading)
            case -4:{  //jmp I
              selectedFile.open(); //Restart from the beggining to search for the label
              char label = thread.getArg(0, 1); //TODO: switch to getRawArg()
              char searching = 1;
              while (searching) {
                if (thread.ignore(selectedFile.readData()) == -3) { //label found, checking ID
                  if (thread.getArg(0, 1) == label) searching = 0;  //right label found, exit jump
                }
              }
            break;}
            case -6:  //pch I/R I/R
              lcdoutput.drawchar(thread.getArg(0, 1), thread.getArg(1, 1));
            break;
            case -7:{ //pst I/R I/R     TODO: Handle >16 char calls
              char size = thread.getArg(1, 1);
              char line[16];
              thread.getProgMem(line, thread.getArg(0, 1), size);
              lcdoutput.printScreen("", line);
            break;}
            case -10: //gst I I
            {
              char inputStr[16];
              stringInput(inputStr, "Input Queried :");
              char destAddr = thread.getArg(0, 1);
              char strSize = thread.getArg(1, 1);
              for (char i = 0; i < 16; i++) {
                if (inputStr[i] == 0) {
                  thread.putMem(0, destAddr + i);
                  break;
                }
                if (i >= strSize) break;
                thread.putMem(inputStr[i], destAddr + i);
              }
              lcdoutput.printScreen("", "");
            }
            break;
          }
        }
      }
    printCurrent();
    } break;

  }
  delay(10);
}


void toString(char *output, char num) {//meant to help debug
  if (num < 0) {
    output[0] = '-';
    num *= -1;
  } else {
    output[0] = ' ';
  }
  output[1] = (num / 100) + 48;
  num %= 100;
  output[2] = (num / 10) + 48;
  num %= 10;
  output[3] = num + 48;
}