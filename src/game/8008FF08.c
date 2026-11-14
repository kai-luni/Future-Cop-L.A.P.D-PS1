/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008FF08\n"
    ".ent func_8008FF08\n"
    "func_8008FF08:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x3F\n"
    "nop\n"
    "nop\n"
    "nop\n"
    ".end func_8008FF08\n"
    ".set reorder\n"
);
