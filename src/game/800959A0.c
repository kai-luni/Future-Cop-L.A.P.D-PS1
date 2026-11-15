// Categories: Graphics > GTE > Lighting | Platform > GTE
#include "game/types.h"

/**
 * @brief Sets the GTE light colour matrix (registers 16-20).
 *
 * @param arg0 Pointer to five 32-bit words: LR1/LR2, LR3/LG1, LG2/LG3,
 *             LB1/LB2 and LB3, written to $16, $17, $18, $19 and $20.
 * @return Nothing.
 *
 * @par Confirmed
 * Five lw into $t0-$t4 followed by five ctc2; the register numbers are the
 * light colour matrix of the GTE, which the colour commands (NCCS, NCDS, ...)
 * use as the light source colours.
 *
 * @par Callers
 * func_80013A14, together with func_800959D0 (background colour) - one
 * lighting set-up per scene.
 */
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
