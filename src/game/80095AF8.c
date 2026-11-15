// Categories: Graphics > GTE > Mathematics | Platform > GTE
/* Handwritten function */
/**
 * @brief SQR without shift: squares the IR vector and returns MAC1..MAC3.
 *
 * @param (implicit) $a0 pointer to IR1..IR3, $a1 destination.
 * @return In $v0: the destination pointer.
 *
 * @par Confirmed mechanics
 * Same sequence as func_80095AD0 but with 0x4AA00428, i.e. SQR with sf = 0 -
 * the results are not divided by 4096, so they keep full precision.
 *
 * @par Callers
 * None in the symbol table (inlined).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095AF8\n"
    ".ent func_80095AF8\n"
    "func_80095AF8:\n"
    "lwc2 $9, 0x0($a0)\n"
    "lwc2 $10, 0x4($a0)\n"
    "lwc2 $11, 0x8($a0)\n"
    "nop\n"
    ".word 0x4AA00428\n"
    "swc2 $25, 0x0($a1)\n"
    "swc2 $26, 0x4($a1)\n"
    "swc2 $27, 0x8($a1)\n"
    "jr $ra\n"
    "addu $v0, $a1, $zero\n"
    ".end func_80095AF8\n"
    ".set reorder\n"
);
