/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CA00\n"
    ".ent func_8008CA00\n"
    "func_8008CA00:\n"
    "addiu     $t2, $zero, 0xC0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2\n"
    ".end func_8008CA00\n"
    ".set reorder\n"
);
