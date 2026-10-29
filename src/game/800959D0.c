#include "game/types.h"

void func_800959D0(s32 a0, s32 a1, s32 a2)
{
    __asm__ volatile(
        "sll $4, $4, 4\n\t"
        "sll $5, $5, 4\n\t"
        "sll $6, $6, 4\n\t"
        "ctc2 $4, $13\n\t"
        "ctc2 $5, $14\n\t"
        "ctc2 $6, $15\n\t"
    );
}
