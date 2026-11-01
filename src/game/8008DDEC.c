#include "game/types.h"

extern u32 func_8008DC40[];
extern u32 D_8008DCC0[];

void func_8008DDEC(void)
{
    register u32 *dst __asm__("$2");
    register u32 *src __asm__("$10");
    register u32 *end __asm__("$9");

    dst = (u32 *)0xDF80;
    src = func_8008DC40 + 4;
    end = D_8008DCC0;
    do {
        *dst++ = *src++;
    } while (src != end);
}
