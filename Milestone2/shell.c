//declare function
int printStr(char* str);
void readSector(char* buffer, int sector);
void printDir();
void getInput(char* buffer);
void clear(char* bufffer, int size);
void split(char* string, char splitter, char result[64][128], int *count);
void cd(char* path);
void strcpm(char* str1, char* str2);

void copyStr(char* str1, char* str2);
int len(char*str);
int RecogCommand(char *str);
//var global
char currentDir = 4;
char curParent;
char dir[1024];
char input[128];



int main (void) {
    int argc;
    char argv[64][128];

    //readd dir
    int itr,itr2;
    int caseCommand;
    char input[128];
    char command[128];
    char arg[128];
    while (1)
    {
        clear(input,128);
        clear(command,128);
        clear(arg,128);
        curParent = currentDir;
        readSector(dir,0x101);
        readSector(dir+512,0x102);
        printDir();
        printStr("$ ");
        interrupt(0x21,0x1,input,0,0);
        for(itr = 0,itr2 = 0; input[itr2] != ' ';itr++,itr2+=1) {
            command[itr] = input[itr2];
        }
        itr2++; //skip white space
        for(itr = 0; input[itr2] != ' ' && input[itr2] != 0; itr++,itr2++) {
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

void cd(char* path){

}