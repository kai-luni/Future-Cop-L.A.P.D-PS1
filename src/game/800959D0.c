// Categories: Graphics > GTE > Depth Cueing | Platform > GTE
#include "game/types.h"

void gGteSetBackColor(s32 a0, s32 a1, s32 a2) __asm__("func_800959D0");

/**
 * @brief Sets the GTE background colour (registers RBK = 13, GBK = 14,
 *        BBK = 15) used by the depth-cueing commands.
 *
 * @param a0 Red component, shifted left by 4 (12-bit fixed point).
 * @param a1 Green component, shifted left by 4.
 * @param a2 Blue component, shifted left by 4.
 * @return Nothing.
 *
 * @par Confirmed
 * `sll $4,$4,4` / `ctc2 $4,$13` and the two siblings; the shift matches the
 * GTE's 12-bit colour format (8 bit value in the high bits of the register).
 *
 * @par Callers
 * func_80013A14 - the scene/lighting set-up writes the fog colour once per
 * scene.
 */
void gGteSetBackColor(s32 a0, s32 a1, s32 a2)
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
