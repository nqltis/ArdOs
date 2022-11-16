/*
  ustrlib.h : Micro String Library
  Contains basic String functions
  nqltis - 02/11/2022
*/

#include "ustrlib.h"

Ustrlib::Ustrlib() {

}

int Ustrlib::strCompare(char *str1, char *str2) {
  int i = 0;
  while(str1[i] == str2[i]) {
    if (!(str1[i] || str2[i])) return 1;  //if both char are null, return true
    i++;
  }
  return 0;
}

int Ustrlib::strSplit(char *buff, char *str, int index) { //split str with / and write n° index in buffer
  int match = 0;
  while (index) {
    if (str[match] == '/') index--;
    else if (!str[match]) return 0; //if not found return 0
    match++;
  }
  int i = 0;
  while (str[match + i] != '/' && str[match + i]) {
    buff[i] = str[match + i];
    i++;
  }
  buff[i] = 0;
  return 1; //success
}

void Ustrlib::strConcat(char *output, char *str2) {
  int j = 0;
  while(output[j]) {
    j++;
  }
  int i = 0;
  while(str2[i]) {
    output[j++] = str2[i++];
  }
  output[j] = 0;
  return;
}