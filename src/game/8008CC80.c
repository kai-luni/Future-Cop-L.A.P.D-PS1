/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CC80\n"
    ".ent func_8008CC80\n"
    "func_8008CC80:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x38\n"
    ".end func_8008CC80\n"
    ".set reorder\n"
);
