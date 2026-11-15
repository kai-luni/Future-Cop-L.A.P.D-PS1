// Categories: Graphics > GTE > Depth Sort | Platform > GTE
/* Handwritten function */
/**
 * @brief AVSZ4 - averages the Z values of four vertices into OTZ (register 7).
 *
 * @param (implicit) $a0..$a3 the four screen Z values; they are written to
 *        registers $16..$19 (SZ0..SZ3).
 * @return In $v0: OTZ, the depth used for ordering-table bucketing.
 *
 * @par Confirmed mechanics
 * Four mtc2 stores, one nop, `0x4B68002E` = AVSZ4 with sf = 1, then mfc2 $v0,$7.
 *
 * @par Callers
 * None in the symbol table (inlined into the renderer).
 *
 * @par Sibling
 * func_80095B20 is the three-vertex form (AVSZ3).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80095B40\n"
    ".ent func_80095B40\n"
    "func_80095B40:\n"
    "mtc2 $a0, $16\n"
    "mtc2 $a1, $17\n"
    "mtc2 $a2, $18\n"
    "mtc2 $a3, $19\n"
    "nop\n"
    ".word 0x4B68002E\n"
    "mfc2 $v0, $7\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095B40\n"
    ".set reorder\n"
);
