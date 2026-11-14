/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008E000\n"
    ".ent func_8008E000\n"
    "func_8008E000:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x3F\n"
    ".end func_8008E000\n"
    ".set reorder\n"
);
