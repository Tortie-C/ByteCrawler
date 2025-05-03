#include <stdio.h>
#include <ctype.h>

//todo account for utf8 being unavailable, maybe use windows graphics pack? linux?
const char horizontalWall[4] = "\u2500";
const char verticalWall[4] = "\u2502";
const char topLeftCorner[4] = "\u250C";
const char topRightCorner[4] = "\u2510";
const char bottomLeftCorner[4] = "\u2514";
const char bottomRightCorner[4] = "\u2518";
const char leftT[4] = "\u251C";
const char rightT[4] = "\u2524";
const char bottomT[4] = "\u252C";

const char headerColor[] = "5;77m";
const char bodyColor[] = "5;235m";
const char bodyAccentColor[] = "5;234m";
const char byteColor[] = "5;241m";
const char indexColor[] = "5;243m";
const char pathColor[] = "5;15m";

extern char path[40];
extern int byteIndex;
extern FILE* fptr;
// extern printf?

void printByteArray(FILE* fptr, long index, int numRows, int numCols, int row, int col) {

    int currentRow = 0;
    int currentCol = 0;

    printf("\033[%d;%df", row, col);

    // coloring
    printf("\x1b[48;%s\x1b[38;%s", bodyColor, byteColor);


    fseek(fptr, index, SEEK_SET);

    int byte;
    while( (byte = fgetc(fptr)) != EOF ) {
        
        // extra space between first 8 bytes and next
        if(currentCol == 7) {
            col++;
        }

        if(currentCol == numCols) {
            currentRow++;
            currentCol = 0;
            col--;
            printf("\033[%d;%df", row + currentRow, col + currentCol);
        }

        if(currentRow == numRows) {
            break;
        }


        // TODO should check for new line characters?
        // coloring
        if(byte == 0) {
            printf("\033[38;5;250m");
        }
        printf("%02x", byte);

        // filter undesireable characters
        // fix this
        // byte < 0x21 || byte > 0x7E
        if(byte < 0x20 || byte > 0x7E) {
            byte = '.';
        }

        // print the character interpretation
        printf("\033[%d;%df%c", row + currentRow, currentCol + 61, (char)byte);
        printf("\033[%d;%df", row + currentRow, col + currentCol * 3 + 3);

        currentCol++;
    }

    // coloring
    printf("\x1b[0m");
}

void printAt(int row, int col, char* string) {
    printf("\x1b[%d;%df%s", row, col, string);
}

// rework clearing console and use separate buffer also do buffer swapping approach
void printUI() {

    // hide the cursor
    printf("\x1b[?25l");

    // print header
    for(int i = 0; i < 2; i++) {
        printf("\x1b[48;%s", headerColor);
        printAt(1 + i, 1, "                                                                              ");
        printf("\x1b[0m");
    }

    printf("\x1b[48;%s\x1b[38;%s", headerColor, pathColor);
    printAt(1, 2, path);
    printf("\x1b[0m");

    printf("\x1b[48;%s\x1b[38;%s", bodyAccentColor, indexColor);
    printAt(3, 1, "  00 01 02 03 04 05 06 07  08 09 0a 0b 0c 0d 0e 0f                            ");
    printf("\x1b[0m");

    // print body
    for(int i = 0; i < 16; i++) {
        printf("\x1b[48;%s", bodyAccentColor);
        printAt(4 + i, 1, " ");
        printf("\x1b[48;%s", bodyColor);
        printf("                                                  ");
        printf("\x1b[48;%s\x1b[38;%s", bodyAccentColor, indexColor);
        printf(" %06x ", byteIndex + i * 16);
        printf("\x1b[48;%s", bodyColor);
        printf("                  ");
        printf("\x1b[48;%s", bodyAccentColor);
        printf(" ");
        printf("\x1b[0m");
    }

    // print data
    printByteArray(fptr, (long)byteIndex, 16, 16, 4, 3);
    
    // show the cursor
    printf("\x1b[?25h");
    

    /*
    // hide the cursor for the print
    printf("\033[?25l");

    // print data
    printf("\033[H"); // clear the console
    printByteArray(fptr, (long)byteIndex, 16, 16, 4, 3);

    // print aestethics
    printf("\033[%d;%df%s", 1, 1, topLeftCorner);
    for(int i = 0; i < 77; i++) {
        printf(horizontalWall);
    }
    printf("%s\n", topRightCorner);

    printf("%s%c%s", verticalWall, ' ', path);
    printf("\033[%d;%df%s\n", 2, 79, verticalWall);

    printf(leftT);
    for(int i = 0; i < 77; i++) {
        printf(horizontalWall);
    }
    printf(rightT);
    printf("\033[%d;%df%s", 3, 52, bottomT);
    printf("\033[%d;%df%s", 3, 59, bottomT);

    for(int i = 0; i < 16; i++) {
        printf("\033[%d;%df%s", i+4, 1, verticalWall);
        printf("\033[%d;%df%s", i+4, 52, verticalWall);
        
        printf("%06x", byteIndex + i * 16);

        printf("\033[%d;%df%s", i+4, 59, verticalWall);
        printf("\033[%d;%df%s", i+4, 79, verticalWall);
    }

    // show the cursor for the print
    printf("\033[?25h");
    */
}