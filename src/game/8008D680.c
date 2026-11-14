/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D680\n"
    ".ent func_8008D680\n"
    "func_8008D680:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4F\n"
    ".end func_8008D680\n"
    ".set reorder\n"
);
