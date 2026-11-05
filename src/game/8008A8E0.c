#include "game/types.h"
extern s32 D_8009C76C;
extern s32 (*D_8009C7AC[])(void);
extern void (*D_8009C728)(s32);
extern void func_8008C890(u32);
extern s32 func_8008ADB4(void);
void func_8008A8E0(void)
{
    s32 result;
    register s32 *index asm("$5") = (s32 *)0x800A0000;
    register s32 n asm("$3");
    register s32 (*fn)(void) asm("$2");
    asm("" : "=r"(index) : "0"(index));
    index = (s32 *)((char *)index - 0x3894);
    asm volatile("" : "=r"(index) : "0"(index));
    n = *index;
    fn = D_8009C7AC[n];
    *index = n + 1;
    result = fn();
    if (result >= 0) {
        if (D_8009C76C != 0) {
            func_8008C890(60);
            if (!func_8008ADB4()) {
                D_8009C728(-3);
            }
        }
        if (D_8009C76C >= 5) {
            register s32 n2 asm("$2") = D_8009C76C - 1;
            register s32 *p asm("$1");
            asm volatile("" : "=r"(n2) : "0"(n2));
            p = (s32 *)0x800A0000;
            asm volatile("" : "=r"(p) : "0"(p));
            *(s32 *)((char *)p - 0x3894) = n2;
        }
    } else {
        D_8009C728(result);
    }
}
