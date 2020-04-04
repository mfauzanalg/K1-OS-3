#include "fileIO.h"

void prnString(char *string) {
  int i = 0;
  while (*(string+i)!='\0')
  {
    interrupt(16,0XE00 + *(string+i),0,0,0); //using vector table 10h and using 0Eh
    i++;
  }
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

void deleteFile(char* path, char parentIndex){
  char map[512];
  char dir[1024];
  char sector[512];
  char temp[15];
  char S;
  int i,j,k,l;

  // read all sectors
  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  readSector(sector,0x103);

  findFileS(path, parentIndex, &S);
  // Target not found or target is a dir
  if (S == 0xFF){
    prnString("cannot remove : Target is Dir or No such file");
    return;
  }

  // Found
  // Clear Map & sector
  for (j = 0; j < 16; j++){
    if(sector[S*16+j] != 0) {
      map[sector[S*16 + j]] = 0x00;
      clear(sector[S*16+j]*512,512);
      sector[S*16+j] = 0x00;
    }
  }
  // looking for dir indeks
  for (i = 0; i < 64; i++){
    if (dir[i*16 +1] == S){
      break;
    }
  }
  // i*16 adalah baris dimana file di simpan di dir

  //clear dir map
  clear(dir+i*16, 16);

  // write hasil clear ke sectors
  writeSector(map,0x100);
  writeSector(dir,0x101);
  writeSector(dir+512,0x102);
  writeSector(sector,0x103);
  return;
}

void findFileS(char* path, char parentIndex, int *S) {
  char dir[1024];
  char temp[15];
  char tempSec[512];
  int i;
  int j;
  int k;
  k = 0;
  
  for(i = 0; *path != '/' && *path != 0x00 && *path != '\r'; i++,path+=1) {
    temp[i] = *path;
    k++;
  }
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
    path+=1;
    findFileS(path,i,S);
  }
  else {
    if(*path == '/') { //ketemu file sebelum parsenya berakhir
      interrupt(0x21, 0, "Directory isn't Valid\r\n", 0, 0);
      (*S) == 0xFF;
      return;
    }
    (*S) = dir[i*16+1];
  }
}
void cat(char * path, char curDir) {
  char map[512];
  char dir[1024];
  char sector[512];
  char tempSec[512];
  char temp[15];
  char S;
  int i,j,k,l;

  // read all sectors
  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  readSector(sector,0x103);

  findFileS(path, curDir, &S);
  // Target not found or target is a dir
  if (S == 0xFF){
    prnString("cannot remove : Target is Dir or No such file");
    return;
  }

  for (j = 0; j < 16; j++){
    if(sector[S*16+j] != 0) {
      readSector(tempSec,sector[S*16+j]);
      prnString(tempSec);
      prnString("\r\n");
    }
  }
}
// void cp(char* src, char* dest, char srcParent, char destParent) {
//     int i, j, k, l;
//     char SectorIndex;
//     char bufferTemp[8192];
//     int lengthsrc;
//     int lengthdest;
//     int check;
//     int check1;
//     char np[16];
//     char np2[16];
//     clear(np2, 16);
//     clear(np,16);
//     clear(bufferTemp,8192);
//     i = 0;
//     lengthsrc = 0;
//     lengthdest = 0;
//     check1 = 0;
//     check = 0;
    
//     if(*src == 'r' && *(src+1) == 'o' && *(src+2) == 'o' && *(src+3) == 't'){
//         srcParent = 0xff;
//     }
//     if(*src != 0 && *src != '/'){
//         for (i = 0; *src != 0 && *src != '/'; i++,src+=1){
//             np[i] = *src;
//         }
//         lengthsrc = i;
//         if(np[0] == 'r' && np[1] == 'o' && np[2] == 'o' && np[3] == 't'){
//             srcParent = 0xff;
//         }else{
//             for ( i = 0; i < 64; i++){
//             //get the parent dir of file
//                 if (dir[i*16] == srcParent){
//                     //iterating and comparing filename
//                     for (j = 2; j < lengthsrc+2 && dir[i*16+j] == np[j-2]; j++){
//                     }
//                     //if same
//                     if (j == lengthsrc+2){
//                         break;
//                     //else continue iterating i
//                     }
//                 }
//             }
//             check = 1;
//         }
//     }
    
//     if(*dest != '/' && *dest != '\r'){
//         for (l = 0; *dest != '/' && *dest != '\r'; l++, dest+=1){
//             np2[l] = *dest;
//         }
//         lengthdest = l;
//         if(np2[0] == 'r' && np2[1] == 'o' && np2[2] == 'o' && np2[3] == 't'){
//             destParent = 0xff;
//         }else{
//             for ( k = 0; k < 64; k++){
//                 //get the parent dir of file
//                 if (dir[k*16] == destParent){
//                     //iterating and comparing filename
//                     for (j = 2; j < lengthdest+2 && dir[k*16+j] == np2[j-2]; j++){
//                     }
//                     //if same
//                     if (j == lengthdest+2){
//                         break;
//                     //else continue iterating k
//                     }
//                 }
//             }
//             check1 = 1; 
//         }
//     }
//     if (i == 64){
//         printStr("\r\nNo such source file or directory\r\n");
//         return;
//     }else if( k == 64){
//         printStr("\r\nNo such dest file or directory\r\n");
//         return;
//     }else{//ketemu
//         if(check == 1){
//             srcParent = i;
//         }
//         if(check1 == 1){
//             destParent = k;
//         }
        
//         if (*src != 0 || *dest != '\r'){
//             if(*src != 0){
//                 src+=1;
//             }
//             if(*dest != '\r'){
//                 dest+=1;
//             }
//             cp(src, dest, srcParent, destParent);
//         }else{
//             SectorIndex = dir[srcParent*16+1];
//             for(i=0;i<16&&sector[SectorIndex*16+i] != 0;i++) {
//                 readSector(bufferTemp+i*512,sector[SectorIndex*16+i]);
//             }
//             k = i;
//             writeFile(bufferTemp,np,k,destParent);
//             writeSector(dir,0x101);
//             writeSector(dir+512,0x102);
//             writeSector(sector,0x103);
//             writeSector(map,0x100);
//         }
//     }    
// }
