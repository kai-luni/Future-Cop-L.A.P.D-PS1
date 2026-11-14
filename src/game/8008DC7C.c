#include "game/types.h"

void func_8008DC7C(void)
{
    register u32 p __asm__("$3");

    if (*(u32 *)(p + 0x1074) & 0x80) {
        do { } while (*(volatile u32 *)(p + 0x1044) & 0x80);
        goto *(void *)*(u32 *)0xDFFC;
    }
}
