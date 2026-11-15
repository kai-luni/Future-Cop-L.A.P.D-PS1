/* Handwritten function */
__asm__(
    ".set noreorder\n"
    ".globl func_80095B20\n"
    ".ent func_80095B20\n"
    "func_80095B20:\n"
    "mtc2 $a0, $17\n"
    "mtc2 $a1, $18\n"
    "mtc2 $a2, $19\n"
    "nop\n"
    ".word 0x4B58002D\n"
    "mfc2 $v0, $7\n"
    "jr $ra\n"
    "nop\n"
    ".end func_80095B20\n"
    ".set reorder\n"
);
