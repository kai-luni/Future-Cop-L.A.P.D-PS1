// Categories: BIOS > Vector Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches to BIOS vector B0 with selector 0x4C.
 *
 *  This assembly stub transfers control directly to the BIOS entry point,
 *  leaving the caller's argument registers available to the selected service.
 *  @par Globals
 *  No named globals are referenced.
 *  @par Control flow
 *  Jumps to address 0xB0 with selector 0x4C set in the delay slot.
 *  @par Calls
 *  No named functions are called; the BIOS vector receives control.
 *  @par Side effects
 *  Depends on the selected BIOS service; the stub sets registers t1 and t2.
 *  @par Callers
 *  Confirmed: func_8008D784.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DC40\n"
    ".ent func_8008DC40\n"
    "func_8008DC40:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4C\n"
    "nop\n"
    ".end func_8008DC40\n"
    ".set reorder\n"
);
