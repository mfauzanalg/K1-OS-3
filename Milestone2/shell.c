// Primitif
// 1. PrinsString (ok)
// 2. PrintInteger
// 3. PrintChar
// 4. len (ok)
// 5. count (ok)
// 6. isSame (ok)
// 7. copy
// 8. find (ok)
// 9. getCommandType
// 10. concat (gak pernah dipake)
// 11. replace (ok)
// 12. split (ok)
// 13. clear (ok)
// 14. searchDir
// 15. search
void printStr(char *string, int newline);
void strLength(char* str, int *len);
int splitString(char* str, char sprt, char yield[32][128]);
void clear(char* buffer, int size);
char strCompare(char *string1, char *string2);
int recognizeCommand(char* str);
void charReplace(char *string, char from, char to);
void count(char *string, char x, int *total);
int charFind(char *string, char x);

void printStr(char *string, int newline){
    interrupt(0x21, 0x00, string, newline, 0);
}

void strLength(char* str, int *len) {
    int k;
    k = 0;
    for(k = 0; *(str+k) != 0;k++);
    (*len) = k;
}

int splitString(char* str, char sprt, char yield[32][128]) { //assuming that the max char size was 128
    int k,i,len,argc;
    i = 0;
    k = 0;
    len = 0;
    argc = 0;

    //clear the buffer for safety
    clear(yield[0][128],128);
    while (str[i] != 0)
    {
        if(str[i] == sprt) {
            yield[argc][len] = 0; //close the string
            len = 0;
            argc++; //new argv detected
            clear(yield[argc][128],128);
        }   
        else {
            yield[argc][len] = str[i]; 
            len++;
        }
        i++;
    }
    yield[argc][len] = 0; //close the string
    return argc;
}

void clear(char* buffer, int size) {
    int i;
    for ( i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
    
}


char strCompare(char *string1, char *string2){
    int len1, len2;
    int i;
    strLength(string1, &len1);
    strLength(string2, &len2);

    if (len1 == len2){
        i = 0;
        while (i < len1){
            if (string1[i] != string2[i]){
                return 0;
            }
            i++;
        }
        return 1;
    }
    else {
        return 0;
    }
}

int recognizeCommand(char* str) {
    int i;
    char cmd[20];
    int argLen;
    clear(cmd,20);
    strLength(str,&argLen);
    //copy string
    for ( i = 0; i < argLen && i < 20; i++) {
        cmd[i] = *(str+i);
    }
    if(strCompare(cmd,"echo")){
        return 1;
    }
    if(strCompare(cmd,"cd")){
        return 2;
    }
    if(strCompare(cmd,"ls")){
        return 3;
    }
    if(cmd[0]=='.' && cmd[1]=='/'){
        return 4;
    }
    else
    {
        return -1;
    }
}
void charReplace(char *string, char from, char to){
    int len, i, temp;
    strLength(string, &len);
    i = 0;
    while (i < len){
        if (string[i] == from){
            string[i] == to;
        }
        i++;
    }
    string[len] = '\0'; 
}

void count(char *string, char x, int *total){
    int len,i;
    *total = 0;
    strLength(string, &len);
    i = 0;
    while (i < len){
        if (string[i] == x){
            *total += 1;
        }
        i++;
    }
}

int charFind(char *string, char x){
    int len, i;
    strLength(string, &x);
    i = 0;
    while (i < len){
        if (string[i] == x){
            return i;
        }
        i++;
    }
    return -1;
}

int main (void) {
    int argc,itr;
    int command;
    char argv[32][128];
    char onShell = 1;
    char userInput[128];
    char currentDirectory[512];
    clear(currentDirectory,512);//safey 1st
    while(onShell) {
        interrupt(0x21,0x0000,currentDirectory,0,0);//print current dir
        interrupt(0x21, 0x0000, "$",0,0); //print dollar sign
        interrupt(0x21, 0x0001, &userInput,0,0); //get user input
        argc = splitString(&userInput,' ',argv);
        //get command
        command = recognizeCommand(argv[0]); // 1 = echo, 2 = cd, 3 = ls, 4 = execute "./"
        if (command == 1) //echo
        {
            for ( itr = 1; itr < argc; itr++)
            {
                interrupt(0x21,0x0000,argv[itr],0,0);
                interrupt(0x21,0x0000," ",0,0);
            }
        }
        if(command == 2) { //cd
            if (argc > 2) {
                printStr("invalid Params",1);
            }
            else if (argc == 1) {
                currentDirectory[0] = 0xff;
            }
            
        }
        
    }
}


