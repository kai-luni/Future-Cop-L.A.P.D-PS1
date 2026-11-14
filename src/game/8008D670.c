/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D670\n"
    ".ent func_8008D670\n"
    "func_8008D670:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4E\n"
    ".end func_8008D670\n"
    ".set reorder\n"
);
