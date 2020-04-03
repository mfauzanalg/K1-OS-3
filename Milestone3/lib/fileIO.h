#ifndef _FILEIO_H_
#define _FILEIO_H_

void clear(char *buffer, int length);
void readFile(char *buffer, char *path, int *result, char parentIndex);
void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void findFileS(char* path, char parentIndex, int *S);

#endif