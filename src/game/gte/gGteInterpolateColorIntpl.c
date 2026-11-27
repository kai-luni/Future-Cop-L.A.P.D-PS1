// Categories: Graphics > GTE > Interpolation | Platform > GTE
/* Handwritten function */
/**
 * @brief INTPL: interpolates a colour from IR0 and the far colour into RGB2.
 *
 * @param (implicit) $a0 pointer to three consecutive words loaded into
 *        registers 9/10/11 (IR1, IR2, IR3), $a1 the IR0 weight (register 8),
 *        $a2 the destination for RGB2 (register 22, 4 bytes).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * lwc2 $9/$10/$11 load IR1..IR3, mtc2 $a1,$8 loads IR0, `0x4A980011` is INTPL
 * with sf = 1, swc2 $22 stores RGB2, jr $ra/nop closes the leaf.
 *
 * @par Callers
 * None in the symbol table - like func_80095B20 the caller was inlined into
 * the renderer (GTE helper layer).
 */
void gGteInterpolateColorIntpl(void) __asm__("func_80095AAC");

__asm__(
    ".set noreorder\n"
    ".globl func_80095AAC\n"
    ".ent func_80095AAC\n"
    "func_80095AAC:\n"
    "lwc2 $9, 0x0($a0)\n"
    "lwc2 $10, 0x4($a0)\n"
    "lwc2 $11, 0x8($a0)\n"
    "mtc2 $a1, $8\n"
    "nop\n"
    ".word 0x4A980011\n"
    "swc2 $22, 0x0($a2)\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095AAC\n"
    ".set reorder\n"
);
