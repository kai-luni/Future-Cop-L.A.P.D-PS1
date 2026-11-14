#include "game/types.h"

void func_8008DC50(void)
{
    register u32 p __asm__("$3");
    register u32 mask __asm__("$2");
    register u32 h __asm__("$15");
    register u32 v __asm__("$24");
    register u32 r __asm__("$25");

    h = *(u16 *)(p + 0xA);
    v = h | mask;
    r = v | 0x12;
    *(u16 *)(p + 0xA) = r;
    {
        register int i __asm__("$8");
        i = 0x28;
        do { i--; } while (i);
    }
}
