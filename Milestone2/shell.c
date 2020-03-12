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

void printStr(char *string, int newline){
    interrupt(0x21, 0x00, string, newline, 0);
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
    int len;
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
        i++
    }
    return -1;
}

int main (void) {
    char onShell = 1;
    char userInput[128];
    while(onShell) {
        interrupt(0x21, 0x0000, "$",0,0); //print dollar sign
        interrupt(0x21, 0x0001, &userInput,0,0); //get user input
        
    }
}


