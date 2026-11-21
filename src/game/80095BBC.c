// Categories: Graphics > GTE > Vector Product | Platform > GTE
/* Handwritten function */
/**
 * @brief OP without shift: cross product with a temporary matrix, sf = 0.
 *
 * @param (implicit) $a0 matrix words, $a1 vector, $a2 destination.
 * @return Nothing.
 *
 * @par Confirmed
 * Identical sequence to func_80095B64, only the command differs:
 * 0x4B70000C (OP, real opcode 0Ch, sf = 0), so the results keep full
 * precision instead of being divided by 4096.
 *
 * @par Callers
 * None in the symbol table (inlined into the surface-normal calculation).
 */
void gGteCrossProductFullPrecisionPreserveRotation(void) __asm__("func_80095BBC");

__asm__(
    ".set noreorder\n"
    ".globl func_80095BBC\n"
    ".ent func_80095BBC\n"
    "func_80095BBC:\n"
    "cfc2 $t5, $0\n"
    "cfc2 $t6, $2\n"
    "cfc2 $t7, $4\n"
    "lw $t0, 0x0($a0)\n"
    "lw $t1, 0x4($a0)\n"
    "lw $t2, 0x8($a0)\n"
    "ctc2 $t0, $0\n"
    "ctc2 $t1, $2\n"
    "ctc2 $t2, $4\n"
    "lwc2 $11, 0x8($a1)\n"
    "lwc2 $9, 0x0($a1)\n"
    "lwc2 $10, 0x4($a1)\n"
    "nop\n"
    ".word 0x4B70000C\n"
    "swc2 $25, 0x0($a2)\n"
    "swc2 $26, 0x4($a2)\n"
    "swc2 $27, 0x8($a2)\n"
    "ctc2 $t5, $0\n"
    "ctc2 $t6, $2\n"
    "ctc2 $t7, $4\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095BBC\n"
    ".set reorder\n"
);
