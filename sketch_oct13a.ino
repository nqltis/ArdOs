byte RS = 13;   //Register Select: H:data L:Instruction 
byte RW = 12;   //H:Read L:Write
byte E = 11;    //Enable Signal
byte DataPin[8] = {
  8, 9, 2, 3, 4, 5, 6, 7
};
byte InputPin[8] = {
  0, 1, A5, A4, A3, A2, A1, A0
};

void pulse() {
  delay(1);
  digitalWrite(E, HIGH);
  delay(1);
  digitalWrite(E, LOW);
  delay(1);
}
void dwrite(byte data){
  for (byte i = 0; i < 8; i++) {
    digitalWrite(DataPin[i], !!(data & (1 << i)));
  }
}
void execinstr(byte data) {
  digitalWrite(RS, LOW);
  dwrite(data);
  pulse();
}
void senddata(byte data) {
  digitalWrite(RS, HIGH);
  dwrite(data);
  pulse();
}
void drawchar(char chr, int pos) {
  if (pos >= 16) pos+= 48;
  execinstr(128 | pos);
  senddata(chr);
}
void printScreen(String line1, String line2) {
  line1 += "                ";
  line2 += "                ";
  execinstr(128);
  for (byte i = 0; i < 16; i++) {
    senddata(line1[i]);
  }
  execinstr(192);
  for (byte i = 0; i < 16; i++) {
    senddata(line2[i]);
  }
}

byte inRead() {
  byte input = 0;
  for (byte i = 0; i < 4; i++) {
    digitalWrite(InputPin[i], LOW);
    delay(1);
    for (byte j = 0; j < 4; j++) {
      if (!digitalRead(InputPin[j + 4])) {
        input = input | (16 << i) | (1 << j);
      }
    }
    digitalWrite(InputPin[i], HIGH);
    delay(1);
  }
  return input;
}

char translate(byte inbyte) {
  switch (inbyte) {
    case 0: return ' ';
    case 17: return '1';
    case 18: return '2';
    case 20: return '3';
    case 24: return 'A';
    case 33: return '4';
    case 34: return '5';
    case 36: return '6';
    case 40: return 'B';
    case 65: return '7';
    case 66: return '8';
    case 68: return '9';
    case 72: return 'C';
    case 129: return '*';
    case 130: return '0';
    case 132: return '#';
    case 136: return 'D';
  }
  return 'n';
}

byte oldinput;
byte newinput;
char button() { //Returns the pressed button on the first cycle of its activation
  newinput = inRead();
  if ((oldinput == 0) && (newinput != 0)) {
    oldinput = newinput;
    return translate(newinput);
  }
  oldinput = newinput;
  return 0;
}

int mempos = 0;
char memory[128] = {
  1, '/', 4, 'h', 'o', 'm', 'e', 6, 's', 'y', 's', 't', 'e', 'm' 
};

String readFileName(int adress) {
  String str = "";
  int endOfName = adress + memory[adress];
  for (int i = adress + 1; i <= endOfName; i++) {
    str = str + memory[i]; 
  }
  return str;
}
int nextFile(int adress) {
  return adress + memory[adress] + 1;
}

String buff1 = "";
String buff2 = "";
void setup() {
  pinMode(RS, OUTPUT);
  pinMode(RW, OUTPUT);
  pinMode(E, OUTPUT);
  digitalWrite(RS, LOW);
  digitalWrite(RW, LOW);
  digitalWrite(E, LOW);
  for (byte i = 0; i < 8; i++) {
    pinMode(DataPin[i], OUTPUT);
    digitalWrite(DataPin[i], LOW);
  }
  execinstr(1);    //Clear the screen
  execinstr(2);    //Return home
  execinstr(6);    //Entry mode
  execinstr(14);   //Display on, Cursor on
  execinstr(20);   //Enable autoshift right
  execinstr(58);   //Function set
  execinstr(12);   //Cursor off

  for (byte i = 0; i < 4; i++) {
    pinMode(InputPin[i], OUTPUT);
    pinMode(InputPin[i + 4], INPUT_PULLUP);
    digitalWrite(InputPin[i], HIGH);
  }
  buff2 = readFileName(mempos);
  printScreen(buff1, buff2);
}

void loop() {
  switch (button()) {
    case 'B':{
      mempos = nextFile(mempos);
      if (memory[mempos]) {
        buff1 = buff2;
        buff2 = readFileName(mempos);
        printScreen(buff1, buff2);
      }
    break;
    }
  }
}
