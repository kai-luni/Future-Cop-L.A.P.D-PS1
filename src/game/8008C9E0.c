#include "game/types.h"

void func_8008C9E0(void)
{
    __asm__ volatile (
        "mfc0 $4, $12\n\t"
        "nop\n\t"
        "ori $4, $4, 0x401\n\t"
        "mtc0 $4, $12\n\t"
        "nop"
        :
        :
        : "$4"
    );
}
