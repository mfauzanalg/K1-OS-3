//declare function
int printStr(char* str);
void readSector(char* buffer, int sector);
void printDir();
void getInput(char* buffer);
void clear(char* bufffer, int size);
void split(char* string, char splitter, char result[64][128], int *count);
void cd(char* path, char prevParent);
int strcmp(char* str1, char* str2);
int len(char* str);
void strcopy(char* str1, char* str2, int length);

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
    char* file;
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
        file = &command[2];
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

int len(char* str){
    int i;
    i = 0;
    while (str[i] != '\0'){
        i+=1;
    }
    return i;
}

int strcmp(char* str1, char* str2){
    int i;
    int len1, len2;
    len1 = len(str1);
    len2 = len(str2);
    int stop;

    i = 0;
    stop = 0;
    if (len1 != len2){
        return 0;
    }
    else{
        while (i < len1 && !stop){
            if (str1[i] != str2[i]){
                stop = 1;
            }
            i++;
        }
        if (stop){
            return 0;
        }
        else{
            return 1;
        }
    }   
}

void strcopy(char* str, char* res, int length){
    int len1, i;
    len1 = len(str);

    for (i = 0; i < len1; i++){
        res[i] = str[i];
    }
    res[len1] = '\0';

}

void cd(char* path, char prevParent){
    int i, j, k;
    int length;
    char np[14];
    clear(np, 14);
    i = 0;
    length = 0;

    while (path[length] != '/' && path[length] != 0){
        length++;
    }

    for (i = 0; i < length; i++){
        np[i] = path[i];
    }

    for ( i = 0; i < 64; i++){
        //get the parent dir of file
        if (dir[i*16] == curParent){
            //iterating and comparing filename
            for (j = 2; j < k+2 && dir[i*16+j] == np[j-2]; j++){
                //
            }
            //if same
            if (j == k+2){
                break;
            }
        //else continue iterating i
        }
    }

    if (i == 64){
        printStr("bash: cd: ");
        printStr(path);
        printStr(": No such file or directoey");
        curParent = prevParent;
        
    }else{//ketemu
        currentDir = i;
        if (*path !='\0'){
            *path += length+1;
            cd(path, prevParent);
        }//else = done
    }
}