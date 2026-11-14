/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D640\n"
    ".ent func_8008D640\n"
    "func_8008D640:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xAB\n"
    ".end func_8008D640\n"
    ".set reorder\n"
);
