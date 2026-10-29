#include "game/types.h"

void func_800959A0(u32 *arg0)
{
    __asm__ volatile(
        "lw $t0, 0($4)\n\t"
        "lw $t1, 4($4)\n\t"
        "lw $t2, 8($4)\n\t"
        "lw $t3, 12($4)\n\t"
        "lw $t4, 16($4)\n\t"
        "ctc2 $t0, $16\n\t"
        "ctc2 $t1, $17\n\t"
        "ctc2 $t2, $18\n\t"
        "ctc2 $t3, $19\n\t"
        "ctc2 $t4, $20"
    );
}
