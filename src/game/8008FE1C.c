#include "game/types.h"

void func_8008FE1C(u32 value)
{
    register u32 *p __asm__("$1");

    p = (u32 *)0x8009CB50;
    *p = value;
}
