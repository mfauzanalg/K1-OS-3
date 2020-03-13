//declare function
int printStr(char* str);
void readSector(char* buffer, int sector);
void printDir();
void getInput(char* buffer);
void clear(char* bufffer, int size);
void copyStr(char* str1, char* str2);
int len(char*str);
int RecogCommand(char *str);
//var global
char currentDir = 4;
char curParent;
char dir[1024];
char input[128];


int main (void) {
    //readd dir
    int itr;
    int caseCommand;
    char input[128];
    char command[128];
    char arg[128];
    while (1)
    {
        clear(input,128);
        clear(command,128);
        clear(arg,128)
        curParent = currentDir;
        readSector(dir,0x101);
        readSector(dir+512,0x102);
        printDir();
        printStr("$ ");
        interrupt(0x21,0x1,input,0,0);
        for(itr = 0; *input != ' ';itr++,input++) {
            command[itr] = *input;
        }
        input++; //skip white space
        for(itr = 0; *input != ' ' && *input != 0; itr++,input++) {
            arg[itr] = *input;
        }
        RecogCommand(command);
    }
}

//functions

int printStr(char* str) {
    char * print = str;
    interrupt(0x21,0,print,0,0);
}

void readSector(char* buffer, int sector) {
    interrupt(0x21,0x0002,buffer,sector,0);
}

void printDir() {
    char printedParent;
    char cDir[14];
    clear(cDir,14);
    if(curParent == 0xFF) {
        printStr("root");
    }
    else {
        printedParent = curParent;
        curParent = dir[curParent*16];
        printDir();
        printStr("/ ");
        copyStr(cDir,(dir+printedParent*16+2));
        printStr(cDir);
    }
}

void getInput(char* buffer) {
    interrupt(0x21,0x0001,buffer,0,0);
}


void clear(char* bufffer, int size) {
    int i;
    for ( i = 0; i < size; i++)
    {
        *(bufffer+i) = 0;
    }
}

int len(char*str) {
    int i = 0;
    for( i = 0; *(str+i) != 0;i++);
    return i;
}

void copyStr(char* str1, char* str2) {
    int i;
    for(i = 0;i < len(str2);i++) {
        str1[i] = str2[i];
    }
}
