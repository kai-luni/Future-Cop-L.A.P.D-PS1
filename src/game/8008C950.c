/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C950\n"
    ".ent func_8008C950\n"
    "func_8008C950:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x70\n"
    ".end func_8008C950\n"
    ".set reorder\n"
);
