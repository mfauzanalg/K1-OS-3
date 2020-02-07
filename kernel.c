/* Ini deklarasi fungsi */
void handleInterrupt21 (int AX, int BX, int CX, int DX);
void printString(char *string);
void readString(char *string);
void readSector(char *buffer, int sector);
void writeSector(char *buffer, int sector);
void readFile(char *buffer, char *filename, int *success);
void clear(char *buffer, int length); //Fungsi untuk mengisi buffer dengan 0
void writeFile(char *buffer, char *filename, int *sectors);
void executeProgram(char *filename, int segment, int *success);

int main(void) {
  makeInterrupt21();
  while (1);
}

void printString(char *string) {
  int i = 0;
  while (string[i]!='\0')
  {
    int AL = string[i];
    int AH = 14; //using 0xE
    int AX = AH*256 + AL;
    interrupt(16,AX,0,0,0); //using vector table 10h and using 0Eh
    i++;
  }
}

void readString (char *string) {
  int i = 0;
  int AH;
  int AL;
  do
  {
    string[i] = interrupt(22,0,0,0,0);//interrut using 0x17 vector table and 0x0 AH or read string 
    if(string[i]=='\b') 
    {
      printString("\b \b"); //delete single existing char before
      if(i>0) {
        i--; //if 1 arr of char deleted num of elmt --
      }
    }
    else
    {
      //print on TTY
      AL = string[i];
      AH = 14;
      int AX = 14*256 + AL;
      interrupt(16,AX,0,0,0); //print thoose char
      i++;
    }
  } while (string[i-1]!='\n'); //end read if enter key pressed
  string[i-1] = '\0'; //set enter key value to null string
}

void handleInterrupt21 (int AX, int BX, int CX, int DX){
  switch (AX) {
    case 0x0:
      printString(BX);
      break;
    case 0x1:
      readString(BX);
      break;
    case 0x2:
      readSector(BX, CX);
      break;
    case 0x3:
      writeSector(BX, CX);
      break;
    case 0x4:
      readFile(BX, CX, DX);
      break;
    case 0x5:
      writeFile(BX, CX, DX);
      break;
    case 0x6:
      executeProgram(BX, CX, DX);
      break;
    default:
      printString("Invalid interrupt");
  }
}
//hello
