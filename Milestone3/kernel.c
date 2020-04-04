#include "interrupt.h"
#include "lib/math.h"
#include "lib/teks.h"
#include "lib/fileIO.h"
#include "lib/folderIO.h"

void cls();

int main(void) {
  
  char input[10];
  int suc;
  char buffer[10240];
  makeInterrupt21();
  bootlogo();
  
  while(1) {
    printString("\r\nSystem Input : \r\n");
    printString("1. Run Default \r\n");
    printString("2. Open shell  \r\n");
    printString("3. Debug  \r\n");
    interrupt(0x21,0x1,input,0,0);
    switch(input[0])
    {
      case '1' : 
        interrupt(0x21, 0x4, buffer, "key.txt", &suc);
        if (suc>0)
        {
          interrupt(0x21,0x0, "Key: ", 0, 0);
          interrupt(0x21,0x0, buffer, 0, 0);
        }
        else
        {
          printString("sdasadsdaa");
          handleInterrupt21(0XFF05, "asd", "a.txt", &suc);
        }
        break;
      case '2' :
        cls();
        handleInterrupt21(0xFF06,"shell",0x3000,&suc);
        break;
      case '3':
        handleInterrupt21(0XFF04, buffer, "test.txt", &suc);
        printString(buffer);
        handleInterrupt21(0XFF05, buffer, "bab.txt", &suc);
        handleInterrupt21(0XFF04, buffer, "bab.txt", &suc);
        printString(buffer);
    }
  }
      
}


void handleInterrupt21 (int AX, int BX, int CX, int DX){
  char AL, AH;
  AL = (char) (AX);
  AH = (char) (AX >> 8);
  switch (AL) {
    case 0x00:
      printString(BX);
      break;
    case 0x01:
      readString(BX);
      break;
    case 0x02:
      readSector(BX, CX);
      break;
    case 0x03:
      writeSector(BX, CX);
      break;
    case 0x04:
      readFile(BX, CX, DX,AH);
      break;
    case 0x05:
      writeFile(BX, CX, DX, AH);
      break;
    case 0x06:
      executeProgram(BX, CX, DX, AH);
      break;
    default:
      printString("Invalid interrupt");
  }
}


void executeProgram(char *filename, int segment, int *success, char parentIndex){
  char buffer[10240];
  int i;
  readFile(&buffer, filename, success, parentIndex);
  if(*success)
  {
    for(i = 0; i < 10240; i++)
    {
      putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
  }else{
    interrupt(0x21, 0, "File not found!\r\n", 0, 0);
  }
}

void cls() {
  int i;
  for (i = 0; i < 10; i++) {
      printString( "\n\n\n\n\n\n\n\n\n\n" );
    }
}