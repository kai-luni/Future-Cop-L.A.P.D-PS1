// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x15 stub.
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; the BIOS routine runs.
 * @par Side effects
 * Whatever BIOS B0:0x15 performs.
 * @par Callers
 * Confirmed: the CD interrupt/timer block.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DAF0\n"
    ".ent func_8008DAF0\n"
    "func_8008DAF0:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x15\n"
    ".end func_8008DAF0\n"
    ".set reorder\n"
);
