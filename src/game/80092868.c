#include "game/types.h"
/* Test the return sequence under a different compiler. */

extern void (*D_8009DDEC)(const char *, ...);
struct Values { s16 values[8]; u8 first; u8 second; };

void func_80092868(struct Values *arg0)
{
    register int a __asm__("$5");
    register int b __asm__("$6");
    register int c __asm__("$7");
    register int last __asm__("$3");
    register void (*print)(const char *, ...) __asm__("$2");
    register char *format __asm__("$4");
    a = arg0->values[0];
    b = arg0->values[1];
    c = arg0->values[2];
    __asm__("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
    last = arg0->values[3];
    __asm__("" : "=r"(last) : "0"(last));
    print = D_8009DDEC;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x291c, a, b, c, last);
    a = arg0->values[4];
    b = arg0->values[5];
    c = arg0->values[6];
    __asm__("" : "=r"(a), "=r"(b), "=r"(c) : "0"(a), "1"(b), "2"(c));
    last = arg0->values[7];
    __asm__("" : "=r"(last) : "0"(last));
    print = D_8009DDEC;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2938, a, b, c, last);
    a = arg0->first;
    __asm__("" : "=r"(a) : "0"(a));
    print = D_8009DDEC;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2954, a);
    a = arg0->second;
    __asm__("" : "=r"(a) : "0"(a));
    print = D_8009DDEC;
    __asm__("" : "=r"(print) : "0"(print));
    format = (char *)0x80010000;
    __asm__("" : "=r"(format) : "0"(format));
    print(format + 0x2960, a);
    __asm__ volatile(".set\tnoreorder\n\t.set  reorder");
}
