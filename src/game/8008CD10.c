/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CD10\n"
    ".ent func_8008CD10\n"
    "func_8008CD10:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2A\n"
    ".end func_8008CD10\n"
    ".set reorder\n"
);
