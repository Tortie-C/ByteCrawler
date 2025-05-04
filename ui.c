#include <stdio.h>
#include <ctype.h>

const char horizontalWall[] = "\u2500";
const char verticalWall[] = "\u2502";
const char topLeftCorner[] = "\u250C";
const char topRightCorner[] = "\u2510";
const char bottomLeftCorner[] = "\u2514";
const char bottomRightCorner[] = "\u2518";
const char leftT[] = "\u251C";
const char rightT[] = "\u2524";
const char bottomT[] = "\u252C";

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

void printBody() {

    int currentRow = 0;
    int currentCol = 0;
    char text[16];

    printf("\x1b[48;%s \x1b[48;%s\x1b[38;%s ", bodyAccentColor, bodyColor, byteColor);

    fseek(fptr, (long)byteIndex, SEEK_SET);

    int byte;
    while( (byte = fgetc(fptr)) != EOF ) {
        
        printf("%02x ", byte);
        if(byte < 0x20 || byte > 0x7E)
            byte = '.';
        text[currentCol] = (char)byte;
        currentCol++;

        if(currentCol == 8)
            printf(" ");

        if(currentCol == 16) {
            // row index
            printf("\x1b[48;%s\x1b[38;%s %06x \x1b[48;%s\x1b[38;%s ", bodyAccentColor, indexColor, byteIndex + currentRow * 16, bodyColor, byteColor);
            // text
            for(int i = 0; i < 16; i++) {
                printf("%c", text[i]);
            }
            printf(" ");
            // end current row
            printf("\x1b[48;%s \x1b[0m\n", bodyAccentColor);
            // set up for next row
            printf("\x1b[48;%s \x1b[48;%s\x1b[38;%s ", bodyAccentColor, bodyColor, byteColor);
            currentCol = 0;
            currentRow++;
        }

        if(currentRow == 16) {
            break;
        }
    }

    // print remainder of line
    if(currentCol != 0) {
        for(int i = 0; i < 16 - currentCol; i++) {
            printf("   ");
        }
        if(currentCol < 8)
            printf(" ");
    }
    printf("\x1b[48;%s\x1b[38;%s %06x \x1b[48;%s", bodyAccentColor, indexColor, byteIndex + currentRow * 16, bodyColor);
    printf("                  ");
    printf("\x1b[48;%s \x1b[0m\n", bodyAccentColor);

    // print blank lines
    for(int i = 0; i < 16 - currentRow; i++) {
        printf("\x1b[48;%s ", bodyAccentColor);
        printf("\x1b[48;%s                                                  ", bodyColor);
        printf("\x1b[48;%s        ", bodyAccentColor);
        printf("\x1b[48;%s                  ", bodyColor);
        printf("\x1b[48;%s \x1b[0m\n", bodyAccentColor);
    }

    printf("\x1b[0m");
}

void printAt(int row, int col, char* string) {
    printf("\x1b[%d;%df%s", row, col, string);
}

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
    printf("\x1b[0m\n");

    // print body
    printBody();
    
    // show the cursor
    printf("\x1b[?25h");
}