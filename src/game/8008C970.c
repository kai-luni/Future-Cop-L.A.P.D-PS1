// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS B0:0x09 stub (deliver event).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the B0 vector.
 * @par Calls
 * None; BIOS B0:0x09 performs the delivery.
 * @par Side effects
 * Delivers the event identified by the caller's arguments.
 * @par Callers
 * Confirmed: func_8002036C, func_8008DFE0 and the card/driver teardown paths.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C970\n"
    ".ent func_8008C970\n"
    "func_8008C970:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x9\n"
    ".end func_8008C970\n"
    ".set reorder\n"
);
