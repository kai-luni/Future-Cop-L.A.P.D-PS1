/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CD20\n"
    ".ent func_8008CD20\n"
    "func_8008CD20:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2B\n"
    ".end func_8008CD20\n"
    ".set reorder\n"
);
