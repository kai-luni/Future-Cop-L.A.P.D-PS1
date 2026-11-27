// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x12 stub (change thread priority).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; BIOS B0:0x12 changes the priority.
 * @par Side effects
 * Changes the running thread's priority.
 * @par Callers
 * Confirmed: the CD interrupt/timer block (func_8008DC40 neighbourhood).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DAC0\n"
    ".ent func_8008DAC0\n"
    "func_8008DAC0:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x12\n"
    ".end func_8008DAC0\n"
    ".set reorder\n"
);
