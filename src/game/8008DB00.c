#include "game/types.h"

extern void *jtbl_800A3E18_main;

void func_8008DB00(void)
{
    register void *target __asm__("$9") = jtbl_800A3E18_main;
    goto *target;
}
