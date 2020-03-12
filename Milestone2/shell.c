// Primitif
// 1. PrinsString
// 2. PrintInteger
// 3. PrintChar
// 4. len (ok)
// 5. count
// 6. isSame (ok)
// 7. copy
// 8. find
// 9. getCommandType
// 10. concat
// 11. replace
// 12. split (ok)
// 13. clear (ok)
// 14. searchDir
// 15. search

void printString(char *string, int newline){
    
}


void strLength(char* str, int *len) {
    int k;
    k = 0;
    for(k = 0; *(str+k) != 0;k++);
    (*len) = k;
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




int main (void) {
    char onShell = 1;
    char userInput[128];
    while(onShell) {
        interrupt(0x21, 0x0000, "$",0,0); //print dollar sign
        interrupt(0x21, 0x0001, &userInput,0,0); //get user input
        
    }
}


