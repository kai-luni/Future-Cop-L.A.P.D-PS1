// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x4B stub (deliver events by class).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; BIOS B0:0x4B performs the delivery.
 * @par Side effects
 * Delivers all events of the given class.
 * @par Callers
 * Confirmed: the CD completion path.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DC30\n"
    ".ent func_8008DC30\n"
    "func_8008DC30:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4B\n"
    ".end func_8008DC30\n"
    ".set reorder\n"
);
