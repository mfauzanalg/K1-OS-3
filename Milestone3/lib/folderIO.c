#include "folderIO.h"
#include "teks.h"

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

void deleteDirectory(char *path, int *success, char parentIndex){
  char map[512];
  char dir[1024];
  char sector[512];
  int i,j,k,l;
  int succ;
  char temp[15];
  char S;

  readSector(map,0x100);
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  readSector(sector,0x103);

  findFileS(path, parentIndex, &S);

  if (S != 0xFF){
    printString("failed to remove : Not a directory");
    *success = -1;
    return;
  }

  succ = 1;
  j = 0;
  for(i = 0; *path != '/' && *path != 0x00; i++,path+=1) {  
      temp[i] = *path;
      k++;
    }

  for (i = 0; i < 64; i++){
    //get the parent dir of file
    if (dir[i*16] == parentIndex){
      //iterating and comparing filename
      for (j = 2; j < k+2 && dir[i*16+j] == temp[j-2]; j++){}
      //if same
      if(j == k+2) {
        break;
      }
      //else continue iterating i
    }
  }

  //folder ada di baris ke [i*16] <- ini adalah indeks foler di dir
  while (j < 64 && succ){
    if (dir[j*16 + 1] == dir[i*16]){ // kalau ada file/folder di dalam folder yang ingin dihapus
      succ = -1;
    }
    j++;
  }

  if (succ == -1){
    printString("failed to remove : Directory not empty");
    *success = -1;
    return;
  }

  if (j == 64){   // berarti si foldernya kosong bisa dihapus
    
  }

}