// Categories: Graphics > GTE > Clipping | Platform > GTE
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief NCLIP: screen-space winding test for three projected vertices.
 *
 * @param (implicit) $a0/$a1/$a2 the three packed screen XY values; they are
 *        written to SXY0/SXY1/SXY2 (registers 12/13/14).
 * @return In $v0: 1 when MAC0 < 0 (the GTE's "negative area" result, i.e. the
 *         triangle faces away), 0 otherwise - the function returns
 *         `slti v0, MAC0, 0`.
 *
 * @par Confirmed mechanics
 * Three mtc2 stores, two nops (the GTE needs two slots after a load), `0x4B400006`
 * = NCLIP (real opcode 06h), mfc2 $24 reads MAC0.
 *
 * @par Likely role
 * The backface/visibility test of the polygon path: the caller skips the
 * primitive when this returns 1.
 *
 * @par Callers
 * func_800648E8.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800648BC\n"
    ".ent func_800648BC\n"
    "func_800648BC:\n"
    ".word 0x48846000   /* mtc2       $a0, $12 */\n"
    ".word 0x48856800   /* mtc2       $a1, $13 */\n"
    ".word 0x48867000   /* mtc2       $a2, $14 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x4B400006   /* nclip */\n"
    ".word 0x4802C000   /* mfc2       $v0, $24 */\n"
    ".word 0x00000000   /* nop */\n"
    ".word 0x28420000   /* slti       $v0, $v0, 0x0 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_800648BC\n"
    ".set reorder\n"
);
