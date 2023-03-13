/*
  ustrlib.h : Micro String Library
  Contains basic String functions
  nqltis - 02/11/2022
*/

#ifndef Ustrlib_h
#define Ustrlib_h

int strCompare(char *str1, char *str2);
int strSplit(char *buff, char *str, int index);
void strConcat(char *output, char *str2);
void strCopy(char *output, char *input);

#endif