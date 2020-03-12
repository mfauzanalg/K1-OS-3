void strLength(char* str, int *len) {
    int k;
    k = 0;
    for(k = 0; *(str+k) != 0;k++);
    (*len) = k;
}

int main (void) {
    char onShell = 1;
    char userInput[128];
    while(onShell) {
        interrupt(0x21, 0x0000, "$",0,0); //print dollar sign
        interrupt(0x21, 0x0001, &userInput,0,0); //get user input
        
    }
}
