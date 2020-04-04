#ifndef _FILEIO_H_
#define _FILEIO_H_

void prnString(char *string);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
void clear(char *buffer, int length);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void deleteFile(char* path,  char parentIndex);
void findFileS(char* path, char parentIndex, int *S);
void cat(char * path, char curDir);

#endif