/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CA10\n"
    ".ent func_8008CA10\n"
    "func_8008CA10:\n"
    "addiu     $t2, $zero, 0xC0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x3\n"
    ".end func_8008CA10\n"
    ".set reorder\n"
);
