/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C970\n"
    ".ent func_8008C970\n"
    "func_8008C970:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x9\n"
    ".end func_8008C970\n"
    ".set reorder\n"
);
