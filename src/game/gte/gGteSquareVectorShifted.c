// Categories: Graphics > GTE > Mathematics | Platform > GTE
/* Handwritten function */
/**
 * @brief SQR with shift: squares the IR vector and returns MAC1..MAC3.
 *
 * @param (implicit) $a0 pointer to IR1..IR3, $a1 destination for the three
 *        32-bit results.
 * @return In $v0: the destination pointer (unchanged).
 *
 * @par Confirmed mechanics
 * lwc2 $9/$10/$11, nop, `0x4AA80428` = SQR with sf = 1 (real opcode 28h), then
 * swc2 $25/$26/$27 store MAC1/MAC2/MAC3.
 *
 * @par Sibling
 * func_80095AF8 is the sf = 0 form (0x4AA00428), i.e. without the final shift.
 *
 * @par Callers
 * None in the symbol table (inlined).
 */
void gGteSquareVectorShifted(void) __asm__("func_80095AD0");

__asm__(
    ".set noreorder\n"
    ".globl func_80095AD0\n"
    ".ent func_80095AD0\n"
    "func_80095AD0:\n"
    "lwc2 $9, 0x0($a0)\n"
    "lwc2 $10, 0x4($a0)\n"
    "lwc2 $11, 0x8($a0)\n"
    "nop\n"
    ".word 0x4AA80428\n"
    "swc2 $25, 0x0($a1)\n"
    "swc2 $26, 0x4($a1)\n"
    "swc2 $27, 0x8($a1)\n"
    "jr $ra\n"
    "addu $v0, $a1, $zero\n"
    ".end func_80095AD0\n"
    ".set reorder\n"
);
