// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x0D stub (open event).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; BIOS B0:0x0D opens the event.
 * @par Side effects
 * Returns the event handle of the opened event in $v0.
 * @par Callers
 * Confirmed: func_8002036C (device handle release), card init paths.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C990\n"
    ".ent func_8008C990\n"
    "func_8008C990:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xD\n"
    ".end func_8008C990\n"
    ".set reorder\n"
);
