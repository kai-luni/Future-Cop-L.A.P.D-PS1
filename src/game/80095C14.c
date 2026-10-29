#include "game/types.h"

void func_80095C14(u32 value) {
    (void)value;
    __asm__ volatile(
        "mtc2 $4,$30\n\t"
        "nop\n\t"
        "nop\n\t"
        "mfc2 $2,$31"
    );
}
