#include "game/types.h"

extern s32 D_8009CBF8[];
extern s32 func_8008E194(s32);

void func_800908A0(void)
{
    register s32 *p __asm__("$16");
    p = D_8009CBF8;
    __asm__ volatile("" : "+r"(p));
    func_8008E194(*p);
    p -= 10;
    __asm__("" : "+r"(p));
    p[9] = 0;
}
