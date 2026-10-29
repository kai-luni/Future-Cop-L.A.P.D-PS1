#include "game/types.h"

extern void *jtbl_800A3E1C_main;

void func_8008DB14(void)
{
    register void *target asm("$9") = jtbl_800A3E1C_main;

    goto *target;
}
