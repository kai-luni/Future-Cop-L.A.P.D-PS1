#include "game/types.h"

void func_80015F14(void *a0, void *a1, void *a2)
{
    __asm__ volatile(
        "lwc2 $0, 0($a0)\n\t"
        "lwc2 $1, 4($a0)\n\t"
        "lwc2 $6, 0($a1)\n\t"
        "nop\n\t"
        "nop\n\t"
        "cop2 0x0108041B\n\t"
        "swc2 $22, 0($a2)\n\t"
        :
        :
        : "memory"
    );
}
