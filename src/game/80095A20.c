// Categories: Graphics > GTE > Matrix Transform | Platform > GTE
/* Handwritten function */
/**
 * @brief MVMVA on the light matrix with the IR vector as input: IR = LM * IR + BK.
 *
 * @param (implicit) $a0 pointer to three words loaded into IR1..IR3
 *        (registers 9/10/11); $a1 destination for the three result words.
 * @return Nothing; IR1..IR3 are written back to *a1 unchanged in count.
 *
 * @par Confirmed mechanics
 * lwc2 $9/$10/$11, one nop, `0x4A4DA412` = MVMVA with sf = 1, mx = 2 (light
 * matrix), v = 3 (IR vector), cv = 1 (add background colour), lm = 1; then
 * swc2 $9/$10/$11 store the transformed IR1..IR3.
 *
 * @par Note
 * The real GTE opcode is the low six bits (12h = MVMVA); the bits at 20-24 are
 * the "fake" opcode and have no effect on the hardware.
 *
 * @par Callers
 * None in the symbol table (inlined into the lighting path).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095A20\n"
    ".ent func_80095A20\n"
    "func_80095A20:\n"
    "lwc2 $9, 0x0($a0)\n"
    "lwc2 $10, 0x4($a0)\n"
    "lwc2 $11, 0x8($a0)\n"
    "nop\n"
    ".word 0x4A4DA412\n"
    "swc2 $9, 0x0($a1)\n"
    "swc2 $10, 0x4($a1)\n"
    "swc2 $11, 0x8($a1)\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095A20\n"
    ".set reorder\n"
);
