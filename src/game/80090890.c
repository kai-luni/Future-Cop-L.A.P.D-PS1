/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_80090890\n"
    ".ent func_80090890\n"
    "func_80090890:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x18\n"
    "nop\n"
    ".end func_80090890\n"
    ".set reorder\n"
);
