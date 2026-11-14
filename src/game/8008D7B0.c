/* Handwritten function: BIOS vector + setter, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D7B0\n"
    ".ent func_8008D7B0\n"
    "func_8008D7B0:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x5B\n"
    "nop\n"
    "lui       $at, %hi(D_8009C800)\n"
    "jr        $ra\n"
    "sw        $a0, %lo(D_8009C800)($at)\n"
    ".end func_8008D7B0\n"
    ".set reorder\n"
);
