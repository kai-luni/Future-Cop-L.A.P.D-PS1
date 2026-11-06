#include "game/types.h"
extern volatile unsigned char D_1F801800, D_1F801803;
extern int D_8009F61C, D_8009F614;
extern int func_8008E174(int, void *);
extern int func_8008E1BC(int, void *, void *);
extern void func_8008CC80(int);
extern void func_8001C27C(int *, int *);
extern int func_8008E2F8(int, void *);
void func_8001C680(void)
{
    register int *p __asm__("$2");
    register int a0 __asm__("$4");
    register char *a1 __asm__("$5");
    register char *a2 __asm__("$6");
    while (func_8008E174(1, 0)) {}
    a0 = 14;
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9f0;
    a2 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a0), "+r"(a1), "+r"(a2));
    D_1F801800 = 0;
    D_1F801803 = 0;
    __asm__ volatile("" : "+r"(a2) : : "memory");
    if (!func_8008E1BC(a0, a1, a2 + 0x798))
        func_8008CC80(-1);
    a0 = 0x800a0000;
    __asm__ volatile("" : "+r"(a0));
    a0 -= 0x9e4;
    __asm__ volatile("" : "+r"(a0));
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    func_8001C27C((int *)a0, (int *)a1);
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    if (!func_8008E2F8(6, a1)) {
        p = (int *)0x80010000;
        __asm__ volatile("" : "+r"(p));
        *p = 0;
    }
}
