/* Handwritten function */
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
