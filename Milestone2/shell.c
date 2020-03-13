//declare function
int printStr(char* str);
void readSector(char* buffer, int sector);
void printDir();
void getInput(char* buffer);
void clear(char* bufffer, int size);
void split(char* string, char splitter, char result[64][128], int *count);

//var global
char currentDir = 0xFF;
char curParent;
char dir[1024];
char input[128];



int main (void) {
    int argc;
    char argv[64][128];

    //readd dir
    while (1)
    {
        clear(&input,128);
        readSector(dir,0x101);
        readSector(dir+512,0x102);
        curParent = currentDir;
        printDir();
        printStr("$");
        interrupt(0x21, 0x1, &input,0,0);
        printStr("\n");
        interrupt(0x21, 0x0, input, 0 ,0);
        printStr("\r\n");

    
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

void split(char* string, char splitter, char result[64][128], int *count){ 
    int i,j,k;  //string[i], result[j][k]
    i = 0;
    j = 0;
    k = 0;

    clear(result[j], 128);
    while (string[i] != '\0'){
        if(string[i] != splitter){
            result[j][k] = string[i];
            k++;
        }
        else{   //kalo splitter
            result[j][k] = '\0';
            j++;
            k = 0;
            clear(result[j], 128);
        }
        i++;
    }
    result[j][k] = '\0';
}
