#include "teks.h"

void printString(char *string) {
  int i = 0;
  while (*(string+i)!='\0')
  {
    interrupt(16,0XE00 + *(string+i),0,0,0); //using vector table 10h and using 0Eh
    i++;
  }
}


void readString (char *string) {
  int i = 0;
  int AH;
  int AL;
  int AX;
  char backspace[4];
  char enter[3];
  backspace[0] = '\b';
  backspace[1] = ' ';
  backspace[2] = '\b';
  backspace[3] = '\0';
  enter[0] = '\r';
  enter[1] = '\n';
  enter[2] = '\0';
  do
  {
    string[i] = interrupt(22,0,0,0,0);//interrut using 0x17 vector table and 0x0 AH or read string 
    if(string[i]=='\b' && i>0) 
    {
      printString(backspace); //delete single existing char before
      i--; //if 1 arr of char deleted num of elmt --
    }
    else if(string[i] == '\b') {
      
    }
    else
    {
      //print on TTY
      interrupt(16,0XE00 + *(string+i),0,0,0);
      i++;
    }
  } while (string[i-1]!='\r' ); //end read if enter key pressed
  printString(enter);
}