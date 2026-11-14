/* Handwritten function: retail body is three loads off $gp with no return. */
__asm__(
    ".set noreorder\n"
    ".globl func_800826A0\n"
    ".ent func_800826A0\n"
    "func_800826A0:\n"
    "lb      $2, 0x660($gp)\n"
    "lb      $4, 0x661($gp)\n"
    "lbu     $3, 0x661($gp)\n"
    ".end func_800826A0\n"
    ".set reorder\n"
);
