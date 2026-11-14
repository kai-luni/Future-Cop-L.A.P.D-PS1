/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DC20\n"
    ".ent func_8008DC20\n"
    "func_8008DC20:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4A\n"
    ".end func_8008DC20\n"
    ".set reorder\n"
);
