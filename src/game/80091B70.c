/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B70\n"
    ".ent func_80091B70\n"
    "func_80091B70:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x18\n"
    ".end func_80091B70\n"
    ".set reorder\n"
);
