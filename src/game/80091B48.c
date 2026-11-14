/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B48\n"
    ".ent func_80091B48\n"
    "func_80091B48:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x72\n"
    "nop\n"
    "nop\n"
    "nop\n"
    ".end func_80091B48\n"
    ".set reorder\n"
);
