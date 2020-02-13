#include "interrupt.h"
#include "divmod.h"

// Funsgi Tambahan
int mod(int a,int b);
int div (int a, int b);
void bootlogo();

int main(void) {
  char* string;
  int succes = 1;
  char buffer[512*20];
  readFile(&buffer,"test.txt",&succes);
  bootlogo();
  makeInterrupt21();
  readString(string);
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
    // case 0x2:
    //   readSector(BX, CX);
    //   break;
    // case 0x3:
    //   writeSector(BX, CX);
    //   break;
    // case 0x4:
    //   readFile(BX, CX, DX);
    //   break;
    // case 0x5:
    //   writeFile(BX, CX, DX);
    //   break;
    // case 0x6:
    //   executeProgram(BX, CX, DX);
    //   break;
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
  int AX,Ah,Al;
  int BX;
  int CX,Ch,Cl;
  int DX;
  Ah = 0x2;//funct to read 
  Al = 0x01;//num of sector to read
  AX = Ah<<2 +   Al;
  BX = buffer; //sector to copy
  Ch = div(sector,36);  //no cylinder in track contain 2 head with 18 sector each
  Cl = mod(sector,18) + 1; //no sector in track (+1 cuz it cant be 0)
  CX = Ch*0x100 + Cl;
  DX = mod(div(sector,18),2) * 0x100; //head and drive
  interrupt(0x13,AX,BX,CX,DX); //interrupt to read Specific Sector
  interrupt(33, 0, BX, 0, 0);
}

void writeSector(char* buffer, int sector) 
{
  int AX,Ah,Al;
  int BX;
  int CX,Ch,Cl;
  int DX;
  Ah = 0x3;//funct to write 
  Al = 0x01;//num of sector to read
  AX = Ah*256 + Al;
  BX = buffer; //sector to copy
  Ch = div(sector,36);  //no cylinder in track contain 2 head with 18 sector each
  Cl = mod(sector,18) + 1; //no sector in track (+1 cuz it cant be 0)
  CX = Ch*256 + Cl;
  DX = mod(div(sector,18),2) * 256; //head and drive
  interrupt(0x13,AX,BX,CX,DX); //interrupt to read Specific Sector
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
  int i;// file
  int j;// name
  
  isFound = 0;
  i = 0;
  //*Load directory sector into 512-byte char array
  //Disk directory sits at sector 2
  clear(buffer,512);
  readSector(buffer, 2);
  //Try to match file name. If not found, return
  while (!isFound && i<16)
  {
     isMatch = 1;
     j = 0;
     while (j < 12 && filename[j]!='\0')
     {
        if (filename[j] != buffer[j+(32*i)]) {
         !isMatch;
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
  for (j = 0; j < 20; i++,j++)
  {
    if (buffer[i+j] == '\0') {
      break;
    }
    else
    {
      readSector(buffer + j*512,buffer[i+j]);
    }
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
  for (int  idx = 0; idx < 16; ++idx)
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
       *sectors = 0;
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
      while(i<255 && space < *sectors)
      {
        if(map[i] = 0)
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
    *sectors = 0;
  }
  
  writeSector(map,1);
  writeSector(dir,2);
}


void executeProgram(char *filename, int segment, int *success){
  int maxSectorSize = 512*20;
  char buffer[maxSectorSize];
  readFile(&*buffer, &*filename, &*success);
  if(*success){
    for(int i = 0; i < maxSectorSize; i++){
      putInMemory(segment, i, buffer[i]);
    }
    launcProgram(segment);
  }
}


