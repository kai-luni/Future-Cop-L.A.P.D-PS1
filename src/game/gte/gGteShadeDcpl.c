// Categories: Graphics > GTE > Depth Cueing | Graphics > Lighting
/* Handwritten function */
/**
 * @brief DCPL: depth-cue/colour-light a vertex: RGB2 = f(IR, RGBC, IR0, far colour).
 *
 * @param (implicit) $a0 pointer to IR1..IR3, $a1 pointer to the incoming colour
 *        (RGBC, register 6), $a2 the IR0 weight, $a3 the destination for RGB2.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * lwc2 $9/$10/$11 (IR1..IR3), lwc2 $6 (RGBC), mtc2 $a2,$8 (IR0), one nop,
 * `0x4A680029` = DCPL with sf = 1 (real opcode 29h), swc2 $22 stores RGB2.
 *
 * @par Callers
 * None in the symbol table; func_80015F14 is the NCCS sibling used by the
 * model renderer.
 */
void gGteShadeDcpl(void) __asm__("func_80095A48");

__asm__(
    ".set noreorder\n"
    ".globl func_80095A48\n"
    ".ent func_80095A48\n"
    "func_80095A48:\n"
    "lwc2 $9, 0x0($a0)\n"
    "lwc2 $10, 0x4($a0)\n"
    "lwc2 $11, 0x8($a0)\n"
    "lwc2 $6, 0x0($a1)\n"
    "mtc2 $a2, $8\n"
    "nop\n"
    ".word 0x4A680029\n"
    "swc2 $22, 0x0($a3)\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095A48\n"
    ".set reorder\n"
);
