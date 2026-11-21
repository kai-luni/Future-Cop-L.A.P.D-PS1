// Categories: Graphics > GTE > Projection | Platform > GTE
/* Handwritten function: retail body has no C shape (hardware/GTE); words transcribed 1:1. */
/**
 * @brief Sets the GTE projection distance (register H, 26).
 *
 * `ctc2 $a0, $26` writes the single argument straight into GTE control register
 * 26, which is H - the projection plane distance that turns the transformed z
 * into the perspective divide input.
 *
 * @param (implicit) $a0 Projection distance in 1/4096 units (the GTE scales it
 *        internally); 0x200 is the value the front-end passes.
 * @return Nothing.
 *
 * @par Callers
 * func_80010030, always together with func_800959F0 (which sets the screen
 * offset), i.e. the two calls are the complete projection set-up.
 */
void gSetGteProjectionDistance(int arg0) __asm__("func_80095A10");

__asm__(
    ".set noreorder\n"
    ".globl func_80095A10\n"
    ".ent func_80095A10\n"
    "func_80095A10:\n"
    ".word 0x48C4D000   /* ctc2       $a0, $26 */\n"
    ".word 0x03E00008   /* jr         $ra */\n"
    ".word 0x00000000   /* nop */\n"
    ".end func_80095A10\n"
    ".set reorder\n"
);
