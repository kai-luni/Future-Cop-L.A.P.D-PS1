/* Handwritten function */
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
