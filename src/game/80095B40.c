/* Handwritten function */
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
