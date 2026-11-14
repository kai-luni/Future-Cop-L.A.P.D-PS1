/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_800966A0\n"
    ".ent func_800966A0\n"
    "func_800966A0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x39\n"
    ".end func_800966A0\n"
    ".set reorder\n"
);
