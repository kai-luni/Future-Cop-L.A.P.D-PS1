/* Handwritten function */
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
