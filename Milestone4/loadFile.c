// loadFile.c
// Michael Black, 2007
//
// Loads a file into the file system
// This should be compiled with gcc and run outside of the OS

#include <stdio.h>
#include <stdlib.h>

void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify file name to load\n");
    return;
  }

  // open the source file
  FILE* loadFil;
  loadFil = fopen(argv[1], "r");
  if (loadFil == 0) {
    printf("File not found\n");
    return;
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the disk map
  char map[512];
  fseek(floppy, 512*0x100, SEEK_SET);
  for (i = 0; i < 512; i++) {
    map[i] = fgetc(floppy);
  }

  // load the directory
  char dir[512];
  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 1024; i++) {
    dir[i] = fgetc(floppy);
  }

  // load sectors
  char sector[512];
  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++){
     sector[i] = fgetc(floppy);
  }

  // find a free entry in the directory
  for (i = 0; i < 1024; i = i + 0x10)
    if (dir[i] == 0 && dir[i+1] == 0 && dir[i+2] == 0){
       break;
    }
  if (i == 1024) {
    printf("Not enough room in directory\n");
    return;
  }
  int dirindex = i;

  // fill the name field with 00s first
  for (i = 2; i < 16; i++){
     dir[dirindex + i] = 0x00;
  }
  // copy the name over
  for (i = 2; i < 16; i++) {
    if (argv[1][i-2] == 0){
       break;
    }
    dir[dirindex + i] = argv[1][i-2];
  }

  //find empty sector in sector buffer
  for(i = 0; i < 32; i++) {
    if(sector[i*0x10] == 0){
       break;
    }
  }
  if(i == 32) {
    printf("No available space for entry\n");
    return;
  }
  int entrySpaceIndex = i;
  dir[dirindex] = (int)strtol(argv[2], NULL, 16);//assume that the file is binded in root directory
  dir[dirindex + 1] = entrySpaceIndex; // S or the Sector Index of the file

  // find free sectors and add them to the file
  int sectcount = 0;
  while (!feof(loadFil)) {
    if (sectcount == 16) {
      printf("Not enough space in directory entry for file\n");
      return;
    }

    // find a free map entry
    for (i = 0; i < 512; i++)
      if (map[i] == 0){
         break;
      }
    if (i == 512) {
      printf("Not enough room for file\n");
      return;
    }

    // mark the map entry as taken
    map[i] = 0xFF;

    // mark the sector in the sectors entry
    sector[entrySpaceIndex * 16 + sectcount] = i;
    sectcount++;

    printf("Loaded %s to sector %d\n", argv[1], i);

    // move to the sector and write to it
    fseek(floppy, i * 512, SEEK_SET);
    for (i = 0; i < 512; i++) {
      if (feof(loadFil)) {
        fputc(0x0, floppy);
        break;
      } else {
        char c = fgetc(loadFil);
        fputc(c, floppy);
      }
    }
  }

  // write the map and directory back to the floppy image
  fseek(floppy, 512 * 0x100, SEEK_SET);
  for (i = 0; i < 512; i++){
    fputc(map[i], floppy);
  }
   

  fseek(floppy, 512 * 0x101, SEEK_SET);
  for (i = 0; i < 512; i++){
    fputc(dir[i], floppy);
  } 
  

  fseek(floppy, 512 * 0x103, SEEK_SET);
  for (i = 0; i < 512; i++){
     fputc(sector[i], floppy);
  }




  fclose(floppy);
  fclose(loadFil);
}
