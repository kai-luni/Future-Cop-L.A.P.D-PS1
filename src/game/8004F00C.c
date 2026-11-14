#include "game/types.h"

void func_8004F00C(u32 a0, u32 a1, u32 a2, u32 a3, u32 *arg)
{
    register u32 *start __asm__("$2") = arg;
    register u32 *p __asm__("$8");
    register u32 i __asm__("$9");
    register u32 v __asm__("$2");
    register u32 x __asm__("$3");
    u32 s;
    __asm__ __volatile__("" : "+r"(start));
    i = 0;
    p = start;
    __asm__ __volatile__("" : "+r"(p), "+r"(i));
    p[1] = (a0 + a1 + 1) >> 1;
    __asm__ __volatile__("" : : : "memory");
    v = (a0 + a2 + 1) >> 1;
    s = (a2 + a3 + a0 + a1 + 2) >> 2;
    p[0] = a0;
    p[2] = v;
    p[3] = s;
    do {
        v = *p;
        i = i + 1;
        v = ((v + 7) >> 4) - 15;
        __asm__("" : "+r"(v));
        x = v >> 31;
        __asm__("" : "+r"(x));
        x = x & v;
        __asm__("" : "+r"(x));
        x = x + 15;
        __asm__("" : "+r"(x));
        *p = x;
        p = p + 1;
    } while (i < 4);
}
