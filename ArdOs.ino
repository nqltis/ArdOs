#include <Arduino.h>
#include "M16input.h"
#include "LCDoutput.h"
#include "Filesysv3.h"
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

File workingDir(0);  //root
File file1;
File file2;

byte fileIndex = 0; //Selected File index in directory

char buff1[16] = "";
char buff2[16] = "";
void setup() {  
  lcdoutput.init();
  workingDir.getName(buff1);
  if (!strCompare(buff1, "/")) { //If first dir isn't root, fs is invalid
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
          lcdoutput.printScreen("Erasing Memory &", "Initializing");
          workingDir.initfs(0);
          inMenu = 0;
        break;
      }
    }
  }
  lcdoutput.printScreen("=== Welcome  ===", "");
  delay(1000);
  file1 = workingDir.getChild();
  printCurrent();
}

void printCurrent() {
  if (file1.isValid()) file1.getName(buff2); 
  else buff2[0] = 0;
  if (file2.isValid()) file2.getName(buff1); 
  else workingDir.pathString(buff1);
  //else buff1[0] = 0;
  lcdoutput.printScreen(buff1, buff2);
  if (file2.isExecutable()) lcdoutput.drawchar('*', 15);
  if (file2.isDir()) lcdoutput.drawchar('>', 15);
  if (file1.isExecutable()) lcdoutput.drawchar('*', 31);
  if (file1.isDir()) lcdoutput.drawchar('>', 31);
}
void selectPrevFile() {
  if (file2.isValid()) {
    file1 = file2;
    file2 = file2.getPrev();
  }
}
void selectNextFile() {
  if (file1.getNext().isValid()) {
    file2 = file1;
    file1 = file1.getNext();
  }
}
void selectPrevDir() {
  workingDir = workingDir.getParent();  //Get parent before child to handle root dir
  file1 = workingDir.getChild();
  file2 = file1.getPrev();
}
void selectNextDir() {
  if (!file1.isDir()) return; //if not dir, return
  workingDir = file1;
  file1 = workingDir.getChild();
  file2 = File();

}
void controlMenu() {
  lcdoutput.printScreen("1:mkf 2:mkd 3:rm", "4:mkex 5:dbg  `/");
  char inMenu = 1;
  while(inMenu) {
    switch (m16input.button()) {
      case 'A':
      case 'B':
        inMenu ^= 2;
        if (inMenu == 1) lcdoutput.printScreen("1:mkf 2:mkd 3:rm", "4:mkex 5:dbg  `/");
        else lcdoutput.printScreen("4:mkex 5:dbg  /`", "6:initfs");
      break;
      case 'C':
      case 'D':
        inMenu = 0;
      break;
      case '1':{  //New file menu
        char inputStr[16];
        stringInput(inputStr, "New file name :");
        if (inputStr[0] == 255) break;
        inputStr[14] = 0;
        lcdoutput.printScreen("Created file :", inputStr);
        workingDir.mkfile(workingDir, inputStr);
        delay(1000);
        
        if (!file1.isValid()) file1 = workingDir.getChild();
        printCurrent();
        inMenu = 0;
      } break;
      case '2':{  //New dir menu
        char inputStr[16];
        stringInput(inputStr, "New dir name :");
        if (inputStr[0] == 255) break;
        inputStr[14] = 0;
        workingDir.mkdir(workingDir, inputStr);
        lcdoutput.printScreen("Created dir :", inputStr);
        delay(1000);
        
        if (!file1.isValid()) file1 = workingDir.getChild();
        printCurrent();
        inMenu = 0;
      } break;
      case '3':{   //remove file
        char str[5];
        lcdoutput.printScreen("", "");
        for (unsigned int i = 224; i < 272; i++) {
          toString(str, (i));
          lcdoutput.printScreen(str, "");
          char val = workingDir.readRawMem(i);
          if (val > 32) lcdoutput.drawchar(val, 31);
          else lcdoutput.drawchar(val + 48, 31);
          delay(1000);
        }
        inMenu = 0;
      } break;
      case '4':{  //New executable menu
        char inputStr[16];
        stringInput(inputStr, "New exe name :");
        if (inputStr[0] == 255) break;
        inputStr[14] = 0;
        workingDir.mkexe(workingDir, inputStr);
        lcdoutput.printScreen("Created exe :", inputStr);
        delay(1000);
        
        if (!file1.isValid()) file1 = workingDir.getChild();
        printCurrent();
        inMenu = 0;
      } break;
      case '5':{  //debug
        char str[5];
        lcdoutput.printScreen("", "");
        for (unsigned int i = 0; i < 24; i++) {
          toString(str, i);
          lcdoutput.printScreen(str, "");
          char val = workingDir.readRawMem(i);
          if (val > 32) lcdoutput.drawchar(val, 31);
          else lcdoutput.drawchar(val + 48, 31);
          delay(1000);
        }
        inMenu = 0;
      } break;
      case '6':   //initfs
        lcdoutput.printScreen("Erasing Memory &", "Initializing");
        workingDir.initfs(0);
        file1 = File();
        inMenu = 0;
      break;
    }
    delay(10);
  }
}

void stringInput(char *output, char *phrase) {
  TypeSession typesession;      //Create new input session
  char key = m16input.button(); //reset input key
  lcdoutput.printScreen(phrase, "");
  while (key != 'D' && key != 'C') {    //press 'D' to continue, 'C' to cancel
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
      case '#': //Shift
        typesession.chgCase();
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
      case '*':
        typesession.numLock();
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
      default:
        typesession.enterKey(key);
        lcdoutput.printScreen(phrase, typesession.inputStr);
      break;
    }
    key = m16input.button();
  }
  if (key == 'C') {
    output[0] = -128;
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
      if (file1.isDir()) selectNextDir(); //if dir, enter, else, edit file
      else editFile(file1);
      printCurrent();
    break;

    case '#':   //Control menu
      controlMenu();
      printCurrent();
    break;
    case '*':{ //Execute file
      if (!file1.isExecutable()) break;
      file1.open();
      ProgExec thread;
      char running = 1;
      while(running) {  //TODO: Check if at end of file
        char progbyte = file1.read();
        char callcode = thread.execute(progbyte);
        /* debug  
        char text[5];
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
            case -2:  //slp I I
              delay(thread.getArg(0, 2)); //waiting for two more arguments
            break;
            case -3: break; //lab I (no action on reading)
            case -4:{  //jmp I
              file1.open(); //Restart from the beggining to search for the label
              char label = thread.getArg(0, 1); //TODO: switch to getRawArg()
              char searching = 1;
              while (searching) { //TODO: handle label missing
                if (thread.ignore(file1.read()) == -3) { //label found, checking ID
                  if (thread.getArg(0, 1) == label) searching = 0;  //right label found, exit jump
                }
              }
            break;}
            case -6:  //pch I I
              lcdoutput.drawchar(thread.getArg(0, 1), thread.getArg(1, 1));
            break;
            case -7:{ //pst I I     TODO: Handle >16 char calls
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

void editFile(File file) {
  file.open();
  toString(buff1, file.read());
  toString(buff2, file.read());
  file.indexDecrement();
  lcdoutput.printScreen(buff1, buff2);
  unsigned char screenCursor = 0;
  lcdoutput.drawchar('<', 5);
  char key = m16input.button(); //reset input key
  while (key != 'C') {
    key = m16input.button();
    switch (key) {
      case 'A': //Scroll up
        if (screenCursor) {
          screenCursor = 0;
        } else if (file.indexDecrement()) {
          file.indexDecrement();
          toString(buff1, file.read());
          toString(buff2, file.read());
          file.indexDecrement();
          lcdoutput.printScreen(buff1, buff2);
        } else file.read();
        lcdoutput.drawchar('<', 5);
        lcdoutput.drawchar(' ', 21);
      break;
      case 'B': //Scroll down
        if (screenCursor) {
          toString(buff1, file.read());
          toString(buff2, file.read());
          file.indexDecrement();
          lcdoutput.printScreen(buff1, buff2);
        } else {
          screenCursor = 1;
        }
        lcdoutput.drawchar(' ', 5);
        lcdoutput.drawchar('<', 21);
      break;
      case 'C': //Exit file
      break;
      case 'D': //Enter new value
        stringInput(buff2, buff1);  //Get user input
        file.write(toNumber(buff2));  //Write input to memory
        file.indexDecrement();      //Read freshly written byte
        toString(buff2, file.read());
        file.indexDecrement();
        lcdoutput.printScreen(buff1, buff2);  //Refresh display
      break;
      default:
      break;
    }
    delay(1);
  }
}

void toString(char *output, int num) {
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
  output[4] = 0;
}

unsigned char toNumber(char *input) {
  unsigned char i = 0;
  unsigned char j = 0;
  unsigned char pow = 100;
  while (input[j] >= 48 && input[j] < 58 && j < 3) {
    i += (input[j] - 48) * pow;
    pow /= 10;
    j++;
  }
  return i;
}