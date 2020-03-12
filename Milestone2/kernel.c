#include "interrupt.h"
#include "divmod.h"

// Funsgi Tambahan
int mod(int a,int b);
int div (int a, int b);
void bootlogo();

int main(void) {
  
  char input[10];int suc;
  char buffer[10240];
  makeInterrupt21();
  bootlogo();
  
  while(1) {
    printString("\r\nSystem Input : \r\n");
    printString("1. Run Default \r\n");
    printString("2. Run App  \r\n");
    printString("3. Debug  \r\n");
    interrupt(0x21,0x1,input,0,0);
    switch(input[0])
    {
      case '1' : 
        interrupt(0x21, 0x4, buffer, "key.txt", &suc);
        if (suc)
        {
          interrupt(0x21,0x0, "Key : ", 0, 0);
          interrupt(0x21,0x0, buffer, 0, 0);
        }
        else
        {
          interrupt(0x21, 0x6, "milestone1", 0x2000, &suc);
        }
        break;
      case '2' :
        interrupt(0x21, 0x6, "file", 0x2000, &suc);
        break;
      case '3':
        handleInterrupt21(0XFF04, buffer, "test.txt", &suc);
        printString(buffer);
  }
    }
      
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
      readFile(BX, CX, DX,(AH));
      break;
    case 0x05:
      //writeFile(BX, CX, DX, AH);
      break;
    case 0x06:
      //executeProgram(BX, CX, DX, AH);
      break;
    default:
      printString("Invalid interrupt");
  }
}


void printString(char *string) {
  int i = 0;
  while (string[i]!='\0')
  {
    
    interrupt(16,0XE00 + string[i],0,0,0); //using vector table 10h and using 0Eh
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


void findFileS(char* path, char parentIndex, int *S) {
  char dir[1024];
  char temp[15];
  int i;
  int j;
  int k;
  k = 0;
  
  for(i = 0; *path != '/' && *path != 0x00; i++,path+=1) {
    
    temp[i] = *path;
    k++;
  }
  path++;
  //read dir Sectors
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  //iterate dir
  for ( i = 0; i < 64; i++)
  {
    //get the parent dir of file
    if (dir[i*16] == parentIndex)
    {
      //iterating and comparing filename
      for ( j = 2; j < k+2 && dir[i*16+j] == temp[j-2]; j++)
      {

      }
      //if same
      if(j == k+2) {
        break;
      }
      //else continue iterating i
    }
  }
  //not Found
  if(i == 64) {
    interrupt(0x21, 0, "File Not Found", 0, 0);
  }
  //found and i*16 is the address
  if(dir[i*16+1] == 0xFF) {
    //continue search S
    findFileS(path,i,S);
  }
  else {
    (*S) = dir[i*16+1];
  }
}

void readFile(char *buffer, char *path, int *result, char parentIndex) {
  char dir[1024];
  char sector[512];
  char temp[15];
  char S;
  int i;
  int j;
  S = 0xFF;
  i = 0;
  //find Sector index in dir
  findFileS(path,parentIndex,&S);
  if(S == 0xFF) {
    (*result) = -1;
    return;
  }
  
  //read sector at sector 0x103
  readSector(sector,0x103);
  j = 0;
  while (j < 16 && sector[S*16+j] != 0 )
  {
    readSector(buffer+j*512,sector[S*16+j]);
    j++;
  }
  (*result) = 1;
    

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
  readFile(buffer, filename, success, 0xFF);
  if(*success)
  {
    for(i = 0; i < 10240; i++)
    {
      putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
  }else{
    interrupt(0x21, 0, "File not found!", 0, 0);
  }
}
