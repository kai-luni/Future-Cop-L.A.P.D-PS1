#include "game/types.h"

void func_80013ECC(u32 *arg0) {
    (void)arg0;
    __asm__ __volatile__(
        "lw $t0, 0($4)\n\t"
        "lw $t1, 4($4)\n\t"
        "lw $t2, 8($4)\n\t"
        "srl $t0, $t0, 4\n\t"
        "srl $t1, $t1, 4\n\t"
        "srl $t2, $t2, 4\n\t"
        "sll $t1, $t1, 16\n\t"
        "or $t1, $t0, $t1\n\t"
        "ctc2 $t1, $8\n\t"
        "ctc2 $t2, $9\n\t"
        "ctc2 $zero, $10\n\t"
        "ctc2 $zero, $11\n\t"
        "ctc2 $zero, $12"
        :
        :
        : "memory"
    );
}
