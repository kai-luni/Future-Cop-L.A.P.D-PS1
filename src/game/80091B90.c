/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B90\n"
    ".ent func_80091B90\n"
    "func_80091B90:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x13\n"
    ".end func_80091B90\n"
    ".set reorder\n"
);
