#ifndef _FOLDERIO_H_
#define _FOLDERIO_H_

void writeDir(char *path, int *sectors, char parentIndex, int* nLength,char* P);
void deleteDirectory(char *path, int *success, char parentIndex);

#endif