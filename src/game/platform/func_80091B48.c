// Categories: BIOS > Vector Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches to BIOS vector 0xA0 with selector 0x72.
 *
 *  This assembly stub transfers control to the BIOS entry point.
 *  The source does not specify the selected service's interface.
 *  @par Globals
 *  No named globals are referenced by the stub.
 *  @par Control flow
 *  Jumps to 0xA0 and sets the selector in the delay slot.
 *  @par Calls
 *  The BIOS vector at 0xA0 receives selector 0x72.
 *  @par Side effects
 *  BIOS service effects are not established by the source.
 *  @par Callers
 *  Confirmed: func_80091614.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B48\n"
    ".ent func_80091B48\n"
    "func_80091B48:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x72\n"
    "nop\n"
    "nop\n"
    "nop\n"
    ".end func_80091B48\n"
    ".set reorder\n"
);
