// Categories: BIOS > Vector Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches to BIOS A0 function 0x13.
 *
 *  Loads the BIOS vector and function selector, then transfers control
 *  to the BIOS with the incoming argument registers unchanged.
 *  The BIOS service defines the arguments and return value.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Jumps to address 0xA0 with selector 0x13 set in the delay slot.
 *  @par Calls
 *  BIOS A0 vector: dispatches the selected service.
 *  @par Side effects
 *  Depend on the BIOS service.
 *  @par Callers
 *  Confirmed: func_80091614.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B90\n"
    ".ent func_80091B90\n"
    "func_80091B90:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x13\n"
    ".end func_80091B90\n"
    ".set reorder\n"
);
