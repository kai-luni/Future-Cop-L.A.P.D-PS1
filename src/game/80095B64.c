// Categories: Graphics > GTE > Vector Product | Platform > GTE
/* Handwritten function */
/**
 * @brief OP with a temporary matrix: cross product of two vectors, results in
 *        MAC1..MAC3.
 *
 * @param (implicit) $a0 pointer to three matrix words, $a1 pointer to the
 *        vector (IR1..IR3), $a2 destination for the three 32-bit results.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The three rotation-matrix words in use ($0, $2, $4) are saved with cfc2, the
 * new matrix is loaded from *a0 with ctc2, the vector is loaded into IR1..IR3
 * (lwc2 $9/$10/$11), `0x4B78000C` = OP with sf = 1 (real opcode 0Ch) computes
 * the cross product, swc2 $25/$26/$27 store MAC1..MAC3 and the saved matrix is
 * restored with ctc2.
 *
 * @par Why the save/restore
 * The command uses the rotation matrix, so the helper has to swap it in and put
 * the caller's matrix back - the whole point of this wrapper.
 *
 * @par Sibling
 * func_80095BBC is the sf = 0 form (0x4B70000C).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095B64\n"
    ".ent func_80095B64\n"
    "func_80095B64:\n"
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
    ".word 0x4B78000C\n"
    "swc2 $25, 0x0($a2)\n"
    "swc2 $26, 0x4($a2)\n"
    "swc2 $27, 0x8($a2)\n"
    "ctc2 $t5, $0\n"
    "ctc2 $t6, $2\n"
    "ctc2 $t7, $4\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095B64\n"
    ".set reorder\n"
);
