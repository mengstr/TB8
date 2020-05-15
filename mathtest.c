#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Based on http://svn.so-much-stuff.com/svn/trunk/pdp8/src/decus/5,8-25/decus-5-25.pdf
void printBits(size_t const size, void const * const ptr) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;

    for (i=size-1;i>=0;i--)
    {
        for (j=7;j>=0;j--)
        {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
        printf(",");
    }
}


uint32_t signExtend24(uint32_t v) {
    if (v&0x00800000) v=v|0xFF800000;
    else              v=v&~0xFF800000;
    return v;
}

uint16_t rnd12(void) {
    static uint64_t num=1;
    num=num*(131072+3);
    return (num>>24)&4095;
}

    
int32_t rnd24(void) {
    uint16_t a,b;
    uint32_t c;
    a=rnd12();
    b=rnd12();
    c=(a<<12)+b;
    c=signExtend24(c);
    return c;    
}

int main(void) {
    for (int i=0; i<1000000; i++) {
        int32_t a,b,cadd,csub,cmul,cdiv;
        a=rnd24();
        b=rnd24()&0777777;
        uint16_t n=rnd12();
        if (n>=04000) b=-b;
        cadd=(a+b); cadd=signExtend24(cadd);
        csub=(a-b); csub=signExtend24(csub);
        cmul=(a*b); cmul=signExtend24(cmul);
        cdiv=(a/b); cdiv=signExtend24(cdiv);
        printf("% 08d % 08d = % 08d % 08d % 08d % 08d\n",a,b,cadd,csub,cmul,cdiv);
    }
}
