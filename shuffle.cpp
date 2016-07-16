#include <windows.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timing {
    int s, l;
};

inline char mask() {
    return rand() % 94 + 33;
}

#define WHITE (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)
#define BLUE (FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#define VERSION "0.1"

inline void printHelp() {
   printf(
      "Usage: shuffle [-hva] [-e attr] [-d attr]\n\n"
      "Options:\n"
      "\th: print this help message and exit\n"
      "\tv: print version and exit\n"
      "\ta: enable decrypting without user input\n"
      "\te: change color of encrypted text\n"
      "\td: change color of decrypted text\n\n"
      "Note: operand after -e/d is a two-digits hex number. Same as [attr] in 'COLOR' command.\n"
   );
}
 
int main(int argc, char **argv) {
    char c;
    bool autoDecrypt = false;
    short unsigned int encryptColor = WHITE, decryptColor = BLUE;
    while ((c = getopt(argc, argv, "e:d:hva")) != -1) {
        switch(c) {
            case 'v':
                printf("shuffle " VERSION "\n");
                return 0;
            case 'h':
                printHelp();
                return 0;
            case 'e':
                if (sscanf(optarg, "%hx", &encryptColor) != 1) {
                    fprintf(stderr, "Error: missing color code after '-e'\n");
                    return 1;
                }
                break;
            case 'd':
                if (sscanf(optarg, "%hx", &decryptColor) != 1) {
                    fprintf(stderr, "Error: missing color code after '-d'\n");
                    return 1;
                }
                break;
            case 'a':
                autoDecrypt = true;
                break;
        }
    }

    HANDLE hStdout, hNew;
    CONSOLE_SCREEN_BUFFER_INFO info;
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hStdout, &info);
    hNew = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hNew);

    int R = info.srWindow.Bottom - info.srWindow.Top + 1,
        C = info.srWindow.Right - info.srWindow.Left + 1, len = R*C;
    CHAR_INFO charBuf[len];
    SMALL_RECT windowSize = {0, 0, C-1, R-1};
    COORD bufSize = {C, R}, charPos = {0, 0}, curPos = {0, 0};
    DWORD written;
    ReadConsoleOutput(hStdout, charBuf, bufSize, charPos, &info.srWindow);
    FillConsoleOutputAttribute(hNew, encryptColor, len, charPos, &written);

    srand(time(0));
    char original[len];
    timing t[len];
    for (int i = 0; i < len; ++i) {
        charBuf[i].Attributes = encryptColor;
        original[i] = charBuf[i].Char.AsciiChar;
        if (original[i] != ' ') {
            char pt = mask();
            WriteConsoleOutputCharacter(hNew, &pt, 1, curPos, &written);
            int rt = rand() % 50, rs = rt / 4, rl = rs * 3;
            t[i].s = (rt > 10 ? rl : rt) * 100;
            t[i].l = (rt > 10 ? rs : 0) * 100;
            Sleep(10);
        }
        if (++curPos.X == C) {
            curPos.X = 0;
            ++curPos.Y;
        }
    }
    --curPos.Y;
    SetConsoleCursorPosition(hNew, curPos);
    if (autoDecrypt)
        Sleep(3000);
    else
        getchar();

    for (int n = 0; n < 40; ++n) {
        for (int i = 0; i < len; ++i)
            if (charBuf[i].Char.AsciiChar != ' ')
                charBuf[i].Char.AsciiChar = mask();
        WriteConsoleOutput(hNew, charBuf, bufSize, charPos, &windowSize);
        Sleep(50);
    }

    bool loop = true;
    while (loop) {
        loop = false;
        curPos.X = 0;
        curPos.Y = 0;
        for (int i = 0; i < len; ++i) {
            if (original[i] != ' ') {
                if (t[i].s > 0) {
                    loop = true;
                    t[i].s -= 100;
                    if (t[i].s % 500 == 0)
                        charBuf[i].Char.AsciiChar = mask();
                } else if (t[i].l > 0) {
                    loop = true;
                    t[i].l -= 100;
                    charBuf[i].Char.AsciiChar = mask();
                } else {
                    charBuf[i].Char.AsciiChar = original[i];
                    WriteConsoleOutputAttribute(hNew, &decryptColor, 1, curPos, &written);
                    original[i] = ' ';
                }
                WriteConsoleOutputCharacter(hNew, &charBuf[i].Char.AsciiChar, 1, curPos, &written);               
            }
            if (++curPos.X == C) {
                curPos.X = 0;
                ++curPos.Y;
            }
        }
        Sleep(50);
    }
    getchar();
    SetConsoleActiveScreenBuffer(hStdout);
    return 0;
}
