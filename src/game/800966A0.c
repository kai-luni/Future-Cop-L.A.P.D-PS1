// Categories: BIOS > Vector Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches to BIOS A0 function 0x39.
 *
 *  Loads the BIOS vector and function selector, then transfers control
 *  to the BIOS with the incoming argument registers unchanged.
 *  The BIOS service defines the arguments and return value.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Jumps to address 0xA0 with selector 0x39 set in the delay slot.
 *  @par Calls
 *  BIOS A0 vector: dispatches the selected service.
 *  @par Side effects
 *  Depend on the BIOS service.
 *  @par Callers
 *  Confirmed: entrypoint.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_800966A0\n"
    ".ent func_800966A0\n"
    "func_800966A0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x39\n"
    ".end func_800966A0\n"
    ".set reorder\n"
);
