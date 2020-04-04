//declare function

int printStr(char* str);
void getInput(char* buffer);
void split(char* string, char splitter, char result[64][128], int *count);
void mv(char* src, char* dest, char srcParent, char destParent);
int len(char* str);
void printDir();
void cd(char* path, char prevParent);
void copyStr(char* str1, char* str2);
void exeFile(char* dir,char* file,char* suc,char* newDir);
void mkdir(char*path,char parent);
void mv(char* src, char* dest, char srcParent, char destParent);

//var global
char dir[1024];
char input[128];
int segmentAvb;
char currentDir = 0xFF;
char curParent;


int main (void) {
    int segmentAvb = 5;
    //read dir
    int itr,itr2,itr3;
    int caseCommand;
    char* file;
    char input[128];
    char newDir;
    char command[128];
    char arg[128];
    char arg2[128];
    char filename[16];

    int suc;
    while (1)
    {
        readSector(dir,0x101);
        readSector(dir+512,0x102);
        clear(input,128);
        clear(command,128);
        clear(arg,128);
        clear(arg2,128);
        curParent = currentDir;

        printDir();
        printStr("$ ");

        interrupt(0x21,0x1,input,0,0);
        //input command
        for(itr = 0,itr2 = 0; input[itr2] != ' ' && input[itr2] != 0;itr++,itr2+=1) {
            command[itr] = input[itr2];
        }

        if(input[itr2] != 0) {
            itr2++; //skip white space
            for(itr = 0; input[itr2] != ' ' && input[itr2] != 0; itr++,itr2++) {
                arg[itr] = input[itr2];
            }
        }

        if(input[itr2] != 0) {
            itr2++; //skip white space
            for(itr = 0; input[itr2] != ' ' && input[itr2] != 0; itr++,itr2++) {
                arg2[itr] = input[itr2];
            }
        }
        if(command[0]=='c' && command[1]=='d' && arg[0] != 0) {
            cd(arg,currentDir);
        }
        else if(command[0]=='m' && command[1]=='k' && command[2]=='d' && command[3]=='i' && command[4]=='r' && arg[0] != 0) {
            mkdir(arg,currentDir);
        }
        else if(command[0]=='r' && command[1]=='m' && command[2]=='d' && command[3]=='i' && command[4]=='r' && arg[0] != 0) {
            deleteDirectory(arg, &suc, currentDir);
        }
        else if(command[0]=='r' && command[1]=='m' && arg[0] != 0) {
            deleteFile(arg,currentDir);
        }
        else if(command[0]=='c' && command[1]=='a' && command[2]=='t' && arg[0] != 0) {
            cat(arg,currentDir);
        }
        else if(command[0]=='m' && command[1]=='v' && arg[0] != 0 && arg2[0] != 0){
            mv(arg, arg2, currentDir, currentDir);
        }
        else if (command[0] == 'l' && command[1] =='s') {
            listcontent(dir, filename, currentDir, itr, itr2);
        }
        
        else if(command[0] == '.' && command[1] == '/' && command[2]!=0) {
            file = &command[2];
            interrupt(0x21,currentDir << 8||0x06,file,segmentAvb*0x1000,&suc);
            segmentAvb++;
        }
        else if(command[0] == '0' && arg[0]==0) {
            file = &command[0];
            // char specialDir = 0x33;
            interrupt(0x21,0x33 << 8 || 0x06,file,segmentAvb*0x1000,&suc);
            segmentAvb++;
        }
        else {
            printStr("\r\nInvalid Command \r\n");
        }
        if(suc == 1) {
            printStr("\r\nSuccess\r\n");
        }

    }
}

//functions

int printStr(char* str) {
    char * print = str;
    interrupt(0x21,0,print,0,0);
}

void printDir() {
    char printedParent;
    char cDir[16];
    clear(cDir,16);
    if(curParent == 0xFF) {
        printStr("root");
    }
    else {
        printedParent = curParent;
        curParent = dir[curParent*16];
        printDir();
        printStr("/");
        copyStr(cDir,(dir+printedParent*16+2));
        printStr(cDir);
    }
}

void getInput(char* buffer) {
    interrupt(0x21,0x0001,buffer,0,0);
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
int len(char* str) {
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

void cd(char* path, char prevParent){
    int i, j, k;
    int length;
    char np[16];
    clear(np,16);
    i = 0;
    length = 0;

    if(*path == '.' && *(path+1) == '.' && *(path+2) == '\r'){
        if(currentDir!=0xFF){
            currentDir = dir[currentDir*16];
        }
        return;
    }
    else {
        //len name
        for (i = 0; *path != '/' && *path != '\r'; i++,path+=1){
            np[i] = *path;
        }
        length = i;

        for ( i = 0; i < 64; i++){
            //get the parent dir of file
            if (dir[i*16] == currentDir){
                //iterating and comparing filename
                for (j = 2; j < length+2 && dir[i*16+j] == np[j-2]; j++){
                }
                //if same
                if (j == length+2){
                    break;
                //else continue iterating i
                }
            }
        }
        if (i == 64){
            printStr("\r\n: No such file or directory\r\n");
            currentDir = prevParent;
            
        }else{//ketemu
            currentDir = i;
            if (*path !='\r'){
                path+=1;
                cd(path, prevParent);
            }//else = done
        }

    } 
}

void exeFile(char* dir,char* file,char* suc, char* newDir) {
    int i, j, length;
    char temp[16];
    length = 0;
    clear(temp,16);
    for(i = 0; *file != '/' && *file != 0; i++,file+=1) {
        temp[i] = *file;
        length++;
    }
    if(*file != 0) {
        file+=1;
    }
    for(i = 0; i < 64; i++) {
        if(dir[i*16] == currentDir) {
            for(j = 2; j <length + 2 && dir[i*16+j] == temp[j-2]; j++)  {
            }
            if(j==length+2) { // is iterator
                break; //i as index of the directory
            }
        }
    }
    if(i!=64) { //
        if(*file != 0) {
            exeFile(i,file,suc,newDir);
        }
        else { //found
            file-=length;
            *newDir = i;
            *suc = 1;
        }
    }
    else { //not found
        *suc = -1;
    }
}

void mv(char* src, char* dest, char srcParent, char destParent){
    int i, j, k, l;
    int lengthsrc;
    int lengthdest;
    int check;
    int check1;
    char np[16];
    char np2[16];
    clear(np2, 16);
    clear(np,16);
    i = 0;
    lengthsrc = 0;
    lengthdest = 0;
    check1 = 0;
    check = 0;
    
    if(*src == 'r' && *(src+1) == 'o' && *(src+2) == 'o' && *(src+3) == 't'){
        srcParent = 0xff;
    }
    if(*src != 0 && *src != '/'){
        for (i = 0; *src != 0 && *src != '/'; i++,src+=1){
            np[i] = *src;
        }
        lengthsrc = i;
        if(np[0] == 'r' && np[1] == 'o' && np[2] == 'o' && np[3] == 't'){
            srcParent = 0xff;
        }else{
            for ( i = 0; i < 64; i++){
            //get the parent dir of file
                if (dir[i*16] == srcParent){
                    //iterating and comparing filename
                    for (j = 2; j < lengthsrc+2 && dir[i*16+j] == np[j-2]; j++){
                    }
                    //if same
                    if (j == lengthsrc+2){
                        break;
                    //else continue iterating i
                    }
                }
            }
            check = 1;
        }
    }
    
    if(*dest != '/' && *dest != '\r'){
        for (l = 0; *dest != '/' && *dest != '\r'; l++, dest+=1){
            np2[l] = *dest;
        }
        lengthdest = l;
        if(np2[0] == 'r' && np2[1] == 'o' && np2[2] == 'o' && np2[3] == 't'){
            destParent = 0xff;
        }else{
            for ( k = 0; k < 64; k++){
                //get the parent dir of file
                if (dir[k*16] == destParent){
                    //iterating and comparing filename
                    for (j = 2; j < lengthdest+2 && dir[k*16+j] == np2[j-2]; j++){
                    }
                    //if same
                    if (j == lengthdest+2){
                        break;
                    //else continue iterating k
                    }
                }
            }
            check1 = 1; 
        }
    }
    if (i == 64){
        printStr("\r\nNo such source file or directory\r\n");
        return;
    }else if( k == 64){
        printStr("\r\nNo such dest file or directory\r\n");
        return;
    }else{//ketemu
        if(check == 1){
            srcParent = i;
        }
        if(check1 == 1){
            destParent = k;
        }
        
        if (*src != 0 || *dest != '\r'){
            if(*src != 0){
                src+=1;
            }
            if(*dest != '\r'){
                dest+=1;
            }
            mv(src, dest, srcParent, destParent);
        }else{
            dir[srcParent*16] = destParent;
            writeSector(dir,0x101);
            writeSector(dir+512,0x102);
        }
    }
}
void mkdir(char* path, char parent) {
    int i, j, k;
    int length;
    char temp[16];
    char np[16];
    clear(np,16);
    clear(temp,16);
    length = 0;
    //check folder is exist or not in CurrentDirectory(parent)
    //len name && copy name
    for (i = 0; *path != '/' && *path != '\r'; i++,path+=1){
        np[i] = *path;
    }
    length = i;

    for (i = 0; i < 64; i++)
    {
        //if some file has parent == 'parent' & is a folder
        if(dir[i*16] == parent && dir[i*16+1] == 0xFF) {
            //compare filename
            for (j = 2; j < length+2 && dir[i*16+j] == np[j-2]; j++){
            }
            //if same
            if(j == length + 2) {
                if(*path != '\r') { //same and path ended in '/' then recurse
                    path+=1;
                    mkdir(path,dir[i*16]); //recurse and change dir
                } else { //same and folder exist and path ended in ''
                    printStr("\r\n mkdir : Cannot create directory, Folder Exist \r\n");
                    return;
                }
            }
            //else continue iterating
        }
    }
    //folder aren't exist
    if(*path != '\r') { //path ended in '/'
        printStr("\r\n No Such File or Directory \r\n");
        return;
    }
    else { //path ended in''
        //create one
        temp[0] = parent; //assign parent of this folder
        temp[1] = 0xFF; //folder flag
        for (j = 2; j < length+2; j++){
            temp[j] = np[j-2];
        }
        //search empty dir
        for(i = 0; i < 64; i++) {
            //search empty space
            if(dir[i*16]==0 && dir[i*16+1]==0 && dir[i*16+2]==0) {
                break;
            }
        }
        //if there is no space left
        if(i == 64) {
            printStr("\r\n mkdir : Insufficent Space\r\n");
            return;
        }
        else {
            for(k = 0; k<2;k++) {
                dir[i*16+k] = temp[k];
            }
            for(k = 0;k<length;k++) {
                dir[i*16+2+k] = np[k];
            }
            writeSector(dir,0x101);
            writeSector(dir+512,0x102);
        }
    }
}

