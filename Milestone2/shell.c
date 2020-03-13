//declare function
int printStr(char* str);
void readSector(char* buffer, int sector);
void printDir();
void getInput(char* buffer);
void clear(char* bufffer, int size);
//var global
char currentDir = 0xFF;
char curParent;
char dir[1024];
char input[128];


int main (void) {
    //readd dir
    while (1)
    {
        clear(&input,128);
        readSector(dir,0x101);
        readSector(dir+512,0x102);
        curParent = currentDir;
        printDir();
        printStr("$");
        interrupt(0x21,1,&input,0,0);
    }
    
}

//functions

int printStr(char* str) {
    char * print = str;
    interrupt(0x21,0,print,0, 0);
}

void readSector(char* buffer, int sector) {
    interrupt(0x21,0x0002,buffer,sector,0);
}

void printDir() {
    char printedParent;
    if(curParent == 0xFF) {
        printStr("root");
    }
    else {
        printedParent = curParent;
        curParent = dir[curParent*16];
        printDir();
        printStr("/");
        printStr(dir[printedParent*16+2]);
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
