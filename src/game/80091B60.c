/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B60\n"
    ".ent func_80091B60\n"
    "func_80091B60:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x17\n"
    ".end func_80091B60\n"
    ".set reorder\n"
);
