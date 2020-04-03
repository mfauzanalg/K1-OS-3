#include "interrupt.h"
#include "lib/math.h"
#include "lib/teks.h"

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


// void printString(char *string) {
//   int i = 0;
//   while (*(string+i)!='\0')
//   {
//     interrupt(16,0XE00 + *(string+i),0,0,0); //using vector table 10h and using 0Eh
//     i++;
//   }
// }


// void readString (char *string) {
//   int i = 0;
//   int AH;
//   int AL;
//   int AX;
//   char backspace[4];
//   char enter[3];
//   backspace[0] = '\b';
//   backspace[1] = ' ';
//   backspace[2] = '\b';
//   backspace[3] = '\0';
//   enter[0] = '\r';
//   enter[1] = '\n';
//   enter[2] = '\0';
//   do
//   {
//     string[i] = interrupt(22,0,0,0,0);//interrut using 0x17 vector table and 0x0 AH or read string 
//     if(string[i]=='\b' && i>0) 
//     {
//       printString(backspace); //delete single existing char before
//       i--; //if 1 arr of char deleted num of elmt --
//     }
//     else if(string[i] == '\b') {
      
//     }
//     else
//     {
//       //print on TTY
//       interrupt(16,0XE00 + *(string+i),0,0,0);
//       i++;
//     }
//   } while (string[i-1]!='\r' ); //end read if enter key pressed
//   printString(enter);
// }

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
    interrupt(0x21, 0, "File Not Found heheheh\r\n", 0, 0);
    (*S) == 0xFF;
    return;
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
  char sector[512];
  char S;
  int i;
  int j;
  S = 0xFF;
  i = 0;
  //find Sector index in dir
  findFileS(path,parentIndex,&S);
  if(S == 0xFF) {
    *result = -1;
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
  *result = 1;
}

void writeDir(char *path, int *sectors, char parentIndex, int* nLength,char* P) {
  char dir[1024];
  char sector[512];
  char map[512];
  char temp[15];

  int i ;
  int length;
  int j;

  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir,0x102);
  readSector(sector,0x103);
  //fill temp with dir or file name
  length = 0;
  clear(temp,15);
  for(i = 0; *path != '/' && *path != 0; i++,path+=1) {
    temp[i] = *path;
    length++;
  }
  if(*path!=0) {
    path+=1;
  }
  //check dir is, file or directory exist or not
  for(i = 0; i < 64; i++) {
    if(dir[i*16] == parentIndex) {
      for(j = 2; j <length + 2 && dir[i*16+j] == temp[j-2]; j++)  {

      }
      if(j==length+2) { // is iterator
        break; //i as index of the directory
      }
    }
  }
  //if found then recurse into the next dir or file
  if(i!=64) {
    if(*path != 0){ //if dir
      writeDir(path,sector,i,nLength,P);
    }
    else { //if file and its exist
      (*sectors) = -1;
      interrupt(0x21, 0, "File already exists!\r\n", 0,0);
    }
  }
  //if not found then create
  else {
    if(*path != 0) { //not found and pointer ends not in the filename which ended by '\0'
      (*sectors) = -4; //error folder not valid
      interrupt(0x21, 0, "Folder is not valid!\r\n", 0,0);
    }
    else { //ended in null string so create the file
      temp[length] = 0; 
      path-=length;
      (*nLength) = length; //length of the file
      (*P) = parentIndex; // parent index of the file or the root
    }
  }
}

void writeFile(char *buffer, char *path, int *sectors, char parentIndex) {
  char map[512];
  char dir[1024];
  char sector[512];
  char buffDir[16];
  char tempBuff[512];
  //var
  int i,k,itr;
  int space;
  //file info
  char filename[15];
  int nameLength;
  char fileParent;

  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  readSector(sector,0x103);

  writeDir(path,sectors,parentIndex,&nameLength,&fileParent);
  //write and search empty sector and the dir ofc
  // P = fileparent, 14 bit name is filename , nameLength for iteration
  if (*sectors == -1 || *sectors == -4) {
    return;
  }
  //else write the file !!

  //find S
  //check space available first
  for ( k = 0; k < 32; k++)
  {
    if(sector[k*16] == 0) break;
  }
  //no space available
  if(k==32) {
    (*sectors) = -3;
     interrupt(0x21, 0, "No space available!\r\n", 0,0);
    return;
  }
  //k == S

  //write P,S,and filename to the dir sector
  //find empty dir
  for(i = 0; i < 64; i++) {
    if(dir[i*16]==0 && dir[i*16 + 1]==0 && dir[i*16 + 2]==0) {
      break;//i as the dir empty index
    }
  }
  //dir is full
  if(i==64) {
     interrupt(0x21, 0, "Dir sector is full!\r\n", 0,0);
    (*sectors) = -2;
    return;
  }
  //dir are available
  //clear(dir+i*16,16);
  dir[i*16] = fileParent;
  dir[i*16+1] = k;
  interrupt(0x21, 0, "awooo", 0,0);
  for(itr = 2; itr < nameLength + 2;itr++) {
    dir[i*16+itr] = *path; //naming the file
    path++;
    
  }
  //naming complete i==useless now
  //write the content to buffer and tags it on sector and map
  clear(sector+k*16,16); //clear sector buffer
  i = 0;
  space = 0;
  while(i<256 && space <*sectors) {
    //space available
    if(map[i] == 0)  {
      sector[k*16+space] = i;
      map[i] = 0xFF;
      //clear tempBuff
      clear(tempBuff,512);
      for ( itr = 0; itr < 512; itr++)
      {
        tempBuff[itr] = buffer[space*512+itr];
      }
      writeSector(tempBuff,i);
      space++;
    }
    i++;
  }

  writeSector(map,0x100);
  writeSector(dir,0x101);
  writeSector(dir+512,0x102);
  writeSector(sector,0x103);
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
    printString("\r\nMasuk\r\n");
    launchProgram(segment);
  }else{
    interrupt(0x21, 0, "File not found!\r\n", 0, 0);
  }
}
