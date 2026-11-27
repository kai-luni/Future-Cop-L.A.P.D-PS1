// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x4A stub (deliver event by class).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; BIOS B0:0x4A performs the delivery.
 * @par Side effects
 * Delivers the event named by the caller's arguments.
 * @par Callers
 * Confirmed: the CD completion path (func_8008DCE8 neighbourhood).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DC20\n"
    ".ent func_8008DC20\n"
    "func_8008DC20:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4A\n"
    ".end func_8008DC20\n"
    ".set reorder\n"
);
