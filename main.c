#include <stdio.h>
#include <windows.h>

#include "ui.c"

int createNewConsole(char* exePath) {

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

    if(!CreateProcess(
        NULL,
        exePath, // this executeable
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE, // create a new console
        NULL,
        NULL,
        &si, // pointer to STARTUPINFO structure
        &pi // pointer to PROCESS_INFORMATION structure
    )) {
        return 1; // indicates that console creation failed and parent process should return
    }

    // wait for child process
    WaitForSingleObject( pi.hProcess, INFINITE );

    // close the process and thead handles once child has exited
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

void processKey(KEY_EVENT_RECORD ker);

char path[40] = "res\\test.txt";
int byteIndex = 0;
FILE* fptr;

int mainLoop = 1;

int main(int argc, char* argv[]) {

    if(GetConsoleWindow() == NULL) {
        return createNewConsole(argv[0]);
    }


    // set up console to print escape sequences and receive input

    // input handle
    HANDLE outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if(outputHandle == INVALID_HANDLE_VALUE) {
        printf("Unable to attain valid output handle");
        return GetLastError();
    }

    // output handle
    HANDLE inputHandle = GetStdHandle(STD_INPUT_HANDLE);
    if(inputHandle == INVALID_HANDLE_VALUE) {
        printf("Unable to attain valid input handle");
        return GetLastError();
    }


    // the console mode being used prior to launching the application should be saved and restored
    DWORD userConsoleOutputMode = 0;
    if(!GetConsoleMode(outputHandle, &userConsoleOutputMode)) {
        printf("Unable to get current console output mode");
        return GetLastError();
    }

    DWORD userConsoleInputMode = 0;
    if(!GetConsoleMode(outputHandle, &userConsoleInputMode)) {
        printf("Unable to get current console input mode");
        return GetLastError();
    }
    

    // enable virtual terminal output 
    if(!SetConsoleMode(outputHandle, userConsoleOutputMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
        printf("Error setting output console mode");
        return GetLastError();
    }

    // enable keyboard and mouse input
    if(!SetConsoleMode(inputHandle, userConsoleInputMode |= ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)) {
        printf("Error setting console input mode");
        return GetLastError();
    }

    // set console to use UTF8 codepage
    unsigned int userCodePage = GetConsoleCP(); // the old code page selected
    if(!SetConsoleCP(CP_UTF8)) {
        return GetLastError();
    }
    if(!SetConsoleOutputCP(CP_UTF8)) {
        return GetLastError();
    }


    // file
    fptr = fopen(path, "rb");

    if(fptr == NULL) {
        printf("Error opening file; did you input a valid path?");
        return 1;
    }

    INPUT_RECORD inputRecordBuffer[128];
    DWORD inputCount;

    // switch to using alternate screen buffer
    printf("\033[?1049h");

    // set window title
    printf("\033]0;ByteCrawler\0x07");

    // main loop
    while(mainLoop) {
        printUI();

        if(!ReadConsoleInput(
            inputHandle, // input bugger handle
            inputRecordBuffer, // buffer to store inputs
            128, // size of buffer
            &inputCount // to hold the number of inputs read
        )) {
            printf("Error reading console input");
            return 1;
        }

        // for every input read from the input buffer
        for(int i = 0; i < inputCount; i++) {

            switch(inputRecordBuffer[i].EventType) {
            case KEY_EVENT:
                processKey(inputRecordBuffer->Event.KeyEvent);
                break;
            default:
                break;
            }
        }
    }

    // return to original console output buffer
    printf("\033[?1049l");


    fclose(fptr); // fptr is use in printUI so it must not be closed until after it has been called

    // return the console's code page to the user's preference
    SetConsoleCP(userCodePage);
    SetConsoleOutputCP(userCodePage);

    // restore console modes
    if(!SetConsoleMode(outputHandle, userConsoleOutputMode)) {
        printf("Error resetting console mode");
    }

    if(!SetConsoleMode(inputHandle, userConsoleInputMode)) {
        printf("Error resetting console mode");
    }

    return 0;
}

void processKey(KEY_EVENT_RECORD ker) {

    WORD keycode = ker.wVirtualKeyCode;
    
    if(ker.bKeyDown) {
        switch(keycode) {
        case VK_ESCAPE:
            mainLoop = 0;
            break;
        case VK_UP:
            byteIndex = byteIndex > 0 ? byteIndex - 0x10 : byteIndex;
            break;
        case VK_DOWN:
            byteIndex += 0x10;
            break;
        }
    }
}

