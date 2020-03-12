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


int main (void) {
    int argc;
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
        
    }
}
