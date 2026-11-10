#include "game/types.h"

extern u8 D_8009DDF2;
extern void (*D_8009DDEC)(u8 *, s32, s32);
extern u8 D_80012AB8[];
extern u32 D_8009DEB0[];
extern u32 D_8009DE9C[];

u32 *func_800930B0(u32 *arg0, s32 arg1)
{
    register u32 *s0 __asm__("$16");
    register s32 s1 __asm__("$17");
    register u32 a2 __asm__("$6");
    register u32 v1 __asm__("$3");
    register u32 a1 __asm__("$5");
    register u32 *v0 __asm__("$2");
    void (*f)(u8 *, s32, s32);
    u32 *p;
    u32 m;

    s0 = arg0;
    s1 = arg1;

    if (D_8009DDF2 >= 2) {
        u8 *msg = D_80012AB8;
        f = D_8009DDEC;
        __asm__ volatile("" : "+r"(msg) : : "memory");
        __asm__ volatile("" : "+r"(s0) : : "memory");
        f(msg, (s32)s0, s1);
    }

    s1 = s1 - 1;
    if (s1 != 0) {
        m = 0xFFFFFF;
        do {
            s1--;
            p = s0 + 1;
            *(u8 *)((u8 *)s0 + 3) = 0;
            *s0 = (*s0 & 0xFF000000) | ((u32)p & m);
            s0 = p;
        } while (s1 != 0);
    }

    a2 = 0xFFFFFF;
    v0 = s0;
    __asm__ volatile("" : "+r"(v0));
    a1 = (u32)D_8009DEB0;
    __asm__ volatile("" : "+r"(a1));
    v1 = (u32)D_8009DE9C;
    v1 = v1 & a2;
    v1 = v1 | 0x4000000;
    *(u32 *)a1 = v1;
    a1 = a1 & a2;
    *v0 = a1;
    return v0;
}
