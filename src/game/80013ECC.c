// Categories: Graphics > GTE > Lighting | Platform > GTE
#include "game/types.h"

/**
 * @brief Loads a light direction into the GTE light matrix and clears the rest.
 *
 * @param arg0 Pointer to three words (x, y, z).
 * @return Nothing.
 *
 * @par Confirmed
 * Each component is shifted right by 4 (the GTE matrix format), then written as
 * L11L12 = ($t0 | $t1 << 16), L13L21 = $t2, and L22L23/L31L32/L33 = 0 via
 * `ctc2 $zero`. The result is a light matrix whose first row is the direction
 * vector and whose remaining rows are zero.
 *
 * @par Callers
 * func_80013F08 and func_8001428C - the lighting set-up before a shaded object
 * is drawn.
 */
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
