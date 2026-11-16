// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten PS1 BIOS stub for the C0 table.
 *
 * The inline assembly loads $t2 = 0xC0 and jumps to it with $t1 = 0x03 in the
 * delay slot, so control leaves the game image through the C0 dispatch vector
 * without touching the caller's arguments.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Unconditional tail jump.
 *
 * @par Calls
 * None; the BIOS routine does the work.
 *
 * @par Side effects
 * Whatever the BIOS routine performs.
 *
 * @par Callers
 * Confirmed: the CD/interrupt block around func_80091270.
 */
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
