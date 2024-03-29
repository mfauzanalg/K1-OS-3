#include "folderIO.h"
void prnStr(char *string) {
  int i = 0;
  while (*(string+i)!='\0')
  {
    interrupt(16,0XE00 + *(string+i),0,0,0); //using vector table 10h and using 0Eh
    i++;
  }
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
  char dir[1024];
  int i,j;
  int length;
  char temp[15];
  char S;
  int found;
  readSector(dir,0x101);
  readSector(dir+512,0x102);
  //save filename and parse,
  for(i = 0;*path!='/' && *path != '\r';i++,path+=1) {
    temp[i] = *path;
  }
  length = i;
  //find it on dir
  for(i = 0;i<64;i++) {
    //check dir with same parentFlag
    if(dir[i*16] == parentIndex) {
      for(j = 2; j<length+2 && dir[i*16+j] == temp[j-2];j++) {
      }
      if(j==length+2) {
        //same dir found
        break;
      }
    }
  }
  if(i == 64) { //not found then throw error
    prnStr("\r\ncannot remove : No Such Dir\r\n");
    return;
  }
  else { //found
    if(*path == '/') { //parse haven't done yet
      if(dir[i*16+1] != 0xFF) { //not a folder
        prnStr("\r\ncannot remove : Target is not a folder\r\n");
        return;
      }
      else { //folder and parse haven't done yet
        path+=1;
        deleteDirectory(path,result,i);
      }
    }
    else { //path == \r' parse done!
      if(dir[i*16+1] != 0xFF) { //not a folder
        prnStr("\r\ncannot remove : Target is not a folder\r\n");
        return;
      }
      else { //valid folder with parent i
        //iterating dir check is folder contains something?
        for(j = 0;j<64;j++) {
          if(dir[j*16] == i){ //folder contains something
            break;
          }
        }
        if(j==64) { //folder doesnt contains anything
          clear(dir+i*16,16);
          writeSector(dir,0x101);
          writeSector(dir+512,0x102);
          *result = 1;
          return;
        } else { //folder filled something
          prnStr("cannot remove : Folder contains file!");
          return;
        }
      }
    }
  }
}

void listcontent(char* dir, char* filename, char currentDir, int itr, int itr2){
  prnStr("\r\n");
  for(itr = 0; itr<64;itr++) {
      if(dir[itr*16] == currentDir) {
          clear(filename,16);
          if(dir[itr*16+1] == 0xFF) { //folder
              filename[0] = '/';
              for(itr2 = 2;dir[itr*16+itr2] !=0;itr2++) {
                  filename[itr2-1] = dir[itr*16+itr2]; 
              }
              prnStr(filename);
              prnStr("\t\t\t\t\t ");
          } else { //file
              for(itr2 = 2;dir[itr*16+itr2] !=0;itr2++) {
                  filename[itr2-2] = dir[itr*16+itr2]; 
              }
              prnStr(filename);
              prnStr("\t\t\t\t\t ");
          }
      }
  }
      prnStr("\r\n");
      prnStr("\r\n");
}
