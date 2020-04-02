// Buat debug
#include <stdio.h>
#include <stdlib.h>

void main(int argc, char* argv[]) {
  int i;

  if (argc < 2) {
    printf("Specify folder name to make\n");
    return;
  }

  char parentIndex = 0xFF;
  if (argc > 2) {
    parentIndex = (int)strtol(argv[2], NULL, 16);
  }

  // open the floppy image
  FILE* floppy;
  floppy = fopen("system.img", "r+");
  if (floppy == 0) {
    printf("system.img not found\n");
    return;
  }

  // load the files
  char files[1024];
  fseek(floppy, 257*512, SEEK_SET);
  for (i = 0; i < 1024; i++) files[i] = fgetc(floppy);

  // find a free entry in files
  for (i = 2; i < 1024; i = i + 0x10)
    if (files[i] == 0) break;
  if (i == 1026) {
    printf("Not enough room in files\n");
    return;
  }
  int filesIndex = i-2;

  // set type to folder
  files[filesIndex+1] = 0xFF;
  // set parentIndex
  files[filesIndex] = parentIndex;
  // fill the name field with 00s first
  for (i = 0; i < 14; i++) files[filesIndex + 2 + i] = 0x00;
  // copy the name over
  for (i = 0; i < 14; i++) {
    if (argv[1][i] == 0) break;
    files[filesIndex + 2 + i] = argv[1][i];
  }

  // write the map and directory back to the floppy image
  fseek(floppy, 257*512, SEEK_SET);
  for (i = 0; i < 1024; i++) fputc(files[i], floppy);

  fclose(floppy);
}