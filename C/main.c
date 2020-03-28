#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(DEBUG) && DEBUG > 0
#define DEBP(fmt, args...) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, ##args)
#else
#define DEBP(fmt, args...) /* Don't do anything in release builds */
#endif

#define NAME "TB8 version "
#define VERSION "0.01"

#define MAXLINENO 4095
#define CODESTART 26

#define KEYBUFLEN 80
uint16_t keybuf[KEYBUFLEN]; // Holds the text being edited
uint16_t keybufP;           // Points to next unused loction in the keybuf[]
uint16_t keybufL;
uint16_t mem[4096];

void DumpVars(void) {
    for (uint16_t i = 0; i < 26; i++) {
        printf("%c=%-4d ", i + 65, mem[i]);
        if (i % 8 == 7) printf("\r\n");
    }
    printf("\r\n");
}

void DumpCode(uint16_t len) {
    printf("         |0|  |1|  |2|  |3|  |4|    |5|  |6|  |7|  |8|  |9|\r\n");
    printf("     +-----------------------------------------------------\r\n");
    for (uint16_t i = CODESTART; i < len; i++) {
        if (i % 10 == 0) printf("%04d | ", i);
        printf("%4d ", mem[i]);
        if (i % 10 == 4) printf("  ");
        if (i % 10 == 9) printf("\r\n");
    }
}

//
//
//
void editline() {
    int c;

    keybufP = 0;
    keybufL = 0;

    printf(">");
    for (;;) {
        c = getchar();
        keybuf[keybufP] = c;
        keybufL = keybufP;
        if (c == 13) return;  // Finish editing line at CR
        if (c == 27) exit(0); // Exit program at ESC
        if (c == 'V' - 64) {  // Ctrl-V dumps all variables
            DumpVars();
            continue;
        }
        if (c == 8 || c == 127) { // BS/DEL deletes the rightmost character
            if (keybufP > 0) {
                keybufP--;
                printf("%c %c", 8, 8);
                continue;
            }
        }
        if (c >= 32) {
            if (keybufP < KEYBUFLEN - 1) {
                keybufP++;
                printf("%c", c);
                continue;
            }
        }
        printf("%c", 7); // BEL on invalid character or too long line
    }
}

int16_t parsenum() {
    int16_t num;
    uint16_t t;
    uint16_t negative;

    num = 0;

    negative = 0;
    if (keybuf[keybufP] == '-') {
        negative = 1;
        keybufP++;
    }

    for (;;) {
        t = keybuf[keybufP] - 48;
        if (t > 9) {
            if (negative) num = -num;
            return num;
        }
        num = (num * 8) + num + num + t;
        keybufP++;
    }
}

//
// Search for specified line number
// Returns address for either of
//      The specified line number
//      The next higher line number if the line number doesn't exist
//      The first free memory address if the line number is higher than all already stored
//
uint16_t findLine(uint16_t lookfor) {
    uint16_t p;
    uint16_t len;
    p = CODESTART;
    for (;;) {
        DEBP("Looking for lineno %d at location %d, here is line %d\r\n", lookfor, p, mem[p]);
        if (mem[p] == 0) return p;
        if (mem[p] >= lookfor) return p;
        len = mem[p + 1];
        p = p + len + 2;
    }
}

//
//
//
void removeLine(uint16_t lookfor) {
    uint16_t pSrc;
    uint16_t pDest;
    uint16_t pLast;
    uint16_t len;

    pDest = findLine(lookfor); // Find location of lineno to remove
    if (pDest == 0) return;    // Line is already not here, so return
    len = mem[pDest + 1];      // TODO
    pSrc = pDest + len + 2;
    pLast = findLine(MAXLINENO); // Find location for the end of code
    DEBP("Deleting %d bytes between location %d to %d\r\n", len, pSrc, pDest);
    for (;;) {
        mem[pDest] = mem[pSrc];
        DEBP("mem[%d] = mem[%d]\r\n", pDest, pSrc);
        if (pSrc == pLast) return;
        pDest++;
        pSrc++;
    }
}

//
//
//
void writeBufAt(uint16_t start, uint16_t lineNo, uint16_t len) {
    DEBP("Writing lineno %d, len %d at %d\r\n", lineNo, len, start);
    mem[start] = lineNo;
    start++;
    mem[start] = len;
    start++;
    for (;;) {
        if (len == 0) return;
        mem[start] = keybuf[keybufP];
        DEBP("Inserting data %d at %d\r\n", keybuf[keybufP], start);
        start++;
        keybufP++;
        len--;
    }
}

//
//  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17
//  A  a  a  a  a  B  b  b  b  b  b  b  Z
//  A  a  a  a  a           B  b  b  b  b  b  b  Z
//
void insertEmptyBlock(uint16_t lineNo, uint16_t len) {
    uint16_t pStart;
    uint16_t pSrc;
    uint16_t pDst;
    uint16_t cnt;

    pStart = findLine(lineNo); // 5
    pSrc = findLine(4095);     // 12
    pDst = pSrc + len + 2;     // 15
    cnt = pSrc - pStart + 1;
    DEBP("pStart=%d pSrc=%d pDst=%d cnt=%d\r\n", pStart, pSrc, pDst, cnt);
    for (;;) {
        if (cnt == 0) break;
        mem[pDst] = mem[pSrc];
        pDst--;
        pSrc--;
        cnt--;
    }
    writeBufAt(pStart, lineNo, len);
}

//
// Handle the LIST command - Expand all the source code lines into human-readable text
//
void Command_LIST() {
    uint16_t p;
    uint16_t len;

    p = CODESTART;
    for (;;) {
        if (mem[p] == 0) return; // We're done when found lineNo zero

        printf("%d", mem[p]); // Print lineNo
        p++;

        len = mem[p]; // Get the length of the line
        p++;

        for (;;) {
            if (len == 0) break;  // Break when reached the end of the line
            printf("%c", mem[p]); // Print a character from the line
            // TODO Expand tokens
            p++;
            len--;
        }
        printf("↲\r\n"); // Print the terminating CRLF after the line
    }
}

//
//
//
int main(int argc, char *argv[]) {
    uint16_t lineNo;

    printf("\r\n%s%s\r\n", NAME, VERSION);

    for (;;) {
        editline();
        printf("\r\n");
        keybufP = 0;
        lineNo = parsenum();
        DEBP("Parsed number is %d from a %d character buffer\r\n", lineNo, keybufL);
        if (lineNo > 0) {
            uint16_t p;
            uint16_t len;
            p = findLine(lineNo);
            DEBP("found lineNo %d at location %d\r\n", lineNo, p);
            len = keybufL - keybufP;
            if (len == 0) {
                removeLine(lineNo);
            } else {
                insertEmptyBlock(lineNo, len);
            }
        }
        Command_LIST();
    }
    return 0;
}
