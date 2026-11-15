/* Handwritten function */
__asm__(
    ".set noreorder\n"
    ".globl bios_A0_49_GPU_cw\n"
    ".ent bios_A0_49_GPU_cw\n"
    "bios_A0_49_GPU_cw:\n"
    ".word 0x240A00A0\n"
    ".word 0x01400008\n"
    ".word 0x24090049\n"
    ".word 0x00000000\n"
    ".end bios_A0_49_GPU_cw\n"
    ".set reorder\n"
);
