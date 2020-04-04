#include "folderIO.h"


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
  readSector(dir+512,0x102);
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

void deleteDirectory(char *path, int *result, char parentIndex){
  char map[512];
  char dir[1024];
  char sector[512];
  int i,j;
  int succ;
  char temp[15];
  char S;
  int found;

  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  readSector(sector,0x103);

  findDirS(path, parentIndex, &S);

  // Target not found or target is a dir
  if (S == 0xFFFF){
    printString("cannot remove : No such file");
    *result = -1;
    return;
  }
  if (S == 0xBBBB){
    printString("cannot remove : Target is not Dir");
    *result = -1;
    return;
  }
  //S adalah indeks dari folder (di dir) 

  // Found
  // Cari tau apakah folder kosong atau tidak
  for (i = 0; i < 64; i++){
    // Dir is not empty
    if (dir[16*i] == S){
      printString("cannot remove : Dir is not empty");
      return;
    }
    // else Dir is empty
  }

  //clear dir and map
  for (j = 0; j < 16; j++){
    map[dir[S*16 + j]] = 0x00;
    dir[S*16 + j] = 0x00;
  }

  // write hasil clear ke sectors
  writeSector(map,0x100);
  writeSector(dir,0x101);
  writeSector(dir+512,0x102);
  writeSector(sector,0x103);
  *result = 1;
  return;
}


void findDirS(char* path, char parentIndex, int *S) {
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
    interrupt(0x21, 0, "Folder Not Found heheheh\r\n", 0, 0);
    (*S) == 0xFFFF;
    return;
  }
  //found and i*16 is the address
  if(dir[i*16+1] == 0xFF) {
    //continue search S
    path+=1;
    if (*path == 0x00){ //path udh abis
      *S = i;
      return;
    }
    findDirS(path,i,S);
  }
  else {
    if(*path == '/') { //ketemu file sebelum parsenya berakhir
      interrupt(0x21, 0, "Directory isn't Valid\r\n", 0, 0);
      (*S) == 0xFFFF;
      return;
    }
    // (*S) = dir[i*16+1];
    (*S) = 0xBBBB; //bukan dir
  }
}