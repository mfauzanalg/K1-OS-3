#include "interrupt.h"
#include "divmod.h"

// Funsgi Tambahan
int mod(int a,int b);
int div (int a, int b);
void bootlogo();

int main(void) {
  int suc = 0;
  char buffer[10240];
  makeInterrupt21();
  bootlogo();
  
  interrupt(0x21, 0x4, buffer, "key.txt", &suc);
  if (suc)
  {
    printString("yoloooo13333333\r\n");
    interrupt(0x21,0x0, "Key : ", 0, 0);
    interrupt(0x21,0x0, buffer, 0, 0);
  }
  else
  {
    printString("yoloooo1234\r\n");
    interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
  }
  while (1);
}

void bootlogo(){
    printString(" __  __        _____  ____  _        _____  _ _          _\r\n");   
    printString("|  \\/  |      / ____|/ __ \\| |      |  __ \\(_) |        | |\r\n");  
    printString("| \\  / |_   _| (___ | |  | | |      | |__) |_| |__   ___| |_\r\n"); 
    printString("| |\\/| | | | |\\___ \\| |  | | |      |  _  /| | '_ \\ / _ \\ __|\r\n");
    printString("| |  | | |_| |____) | |__| | |____  | | \\ \\| | |_) |  __/ |_\r\n"); 
    printString("|_|  |_|\\__, |_____/ \\___\\_\\______| |_|  \\_\\_|_.__/ \\___|\\__|\r\n");
    printString("         __/ |\r\n");                                               
    printString("        |___/\r\n");   
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      readFile(BX, CX, DX);
      break;
    case 0x5:
      writeFile(BX, CX, DX);
      break;
    case 0x6:
      executeProgram(BX, CX, DX);
      break;
    default:
      printString("Invalid interrupt");
  }
}


void printString(char *string) {
  int i = 0;
  while (string[i]!='\0')
  {
    int AL = string[i];
    int AH = 14; //using 0xE
    int AX = AH*256 + AL;
    interrupt(16,AX,0,0,0); //using vector table 10h and using 0Eh
    i++;
  }
}


void readString (char *string) {
  int i = 0;
  int AH;
  int AL;
  int AX;
  do
  {
    string[i] = interrupt(22,0,0,0,0);//interrut using 0x17 vector table and 0x0 AH or read string 
    if(string[i]=='\b') 
    {
      printString("\b \b"); //delete single existing char before
      if(i>0) {
        i--; //if 1 arr of char deleted num of elmt --
      }
    }
    else
    {
      //print on TTY
      AL = string[i];
      AH = 0xE;
      AX  = AH*256 + AL;
      interrupt(16,AX,0,0,0); //print thoose char
      i++;
    }
  } while (string[i-1]!='\r'); //end read if enter key pressed
  string[i-1] = '\0'; //set enter key value to null string
}


int mod(int a,int b) //a mod b
{
  int mod;
  mod = a;
  while (mod >= b) {
    mod-=b;
  }
  return mod;
}


int div (int a, int b) //a div b
{
  int div;
  div  = 0;
  while (div * b <= a) {
    div++;
  }
  return div - 1;
}

void readSector(char* buffer, int sector) 
{
  interrupt(0x13, 0x201, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}

void writeSector(char* buffer, int sector) 
{
  interrupt(0x13, 0x301, buffer, div(sector, 36) * 0x100 + mod(sector, 18) + 1, mod(div(sector, 18), 2) * 0x100);
}


void clear(char *buffer, int length){
  int i;
  for (i = 0; i < length; i++){
    *(buffer+i) = 0;
  }
}

void readFile(char *buffer, char *filename, int *success) {
  int isFound;
  int isMatch;
  int sectorNo; 
  int numSectors;
  char dir [512];
  int i;// file
  int j;// name
  
  isFound = 0;
  i = 0;
  //*Load directory sector into 512-byte char array
  //Disk directory sits at sector 2
  readSector(dir, 2);
  //Try to match file name. If not found, return
  while (!isFound && i<16)
  {
     isMatch = 1;
     j = 0;
     while (j < 12 && filename[j]!='\0')
     {
        if (filename[j] != dir[j+(32*i)]) {
         isMatch = 0;
         break;
        }
        j = j+1;
     }
     if (isMatch) 
     {
       isFound = 1;
     }
     else 
     {
       i = i+1;
     }
  }
  if (!isFound) {
   *success = 0;
   return;
  }
  i=(i*32)+12;
  j = 0;
  numSectors = 0;
  while(j<20 && dir[i+j]!='\0') {
    readSector(buffer+j*512,dir[i+j]);
    j++;
  }
  *success = 1;
}

void writeFile(char *buffer, char *filename, int *sectors) {
  char map[512];
  char dir[512];
  char secBuff[512];
  int idx;

  readSector(map,1);
  readSector(dir,2);

  //find some empty dir 
  for (idx = 0; idx < 16; ++idx)
  {
    if (dir[idx*32] == '\0') 
    {
      break;
    }
  }

  if (idx<16) 
  {
    int space = 0;
    int i = 0;
    int j;
    //find space available
    while (i<256 && space < *sectors)
    {
      if (map[i] == 0)
      {
        space++;
      }
      i++;
    }
    //if there is no space
    if (space < *sectors)
    {
       *sectors = -1;
       return;
    }
    //if there is a space
    else
    {
      //clear file sector
      clear(dir+idx*32,32);
      i = 0;
      while (i<12)
      {
        //if not null string
        if (filename[i] != '\0')
        {
          dir[idx*32+i] = filename[i];
        }
        //case end name
        else
        {
          break;
        }
        i++;
      }
      i = 0;
      space = 0;
      //find space at map
      while(i<256 && space < *sectors)
      {
        if(map[i] == 0)
        {
          map[i] = 0xFF;//sign its used to store data
          dir[idx*32+12+space] = i;
          //clear dir
          clear(secBuff,512);
          j = 0;
          while (j<512)
          {
            secBuff[j] = buffer[space*512+j];
            j++;
          }
          //write sectors
          writeSector(secBuff,i);
          space++;
        }
        i++;
      }
    }    
  }
  else
  {
    *sectors = -1;
    return;
  }
  
  writeSector(map,1);
  writeSector(dir,2);
}


void executeProgram(char *filename, int segment, int *success){
  char buffer[10240];
  int i;
  readFile(buffer, filename, success);
  if(*success)
  {
    for(i = 0; i < 10240; i++)
    {
      putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
  }
}
