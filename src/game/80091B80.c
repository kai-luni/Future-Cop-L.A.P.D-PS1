// Categories: BIOS > Vector Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches to BIOS vector B0 with selector 0x19.
 *
 *  This assembly stub transfers control directly to the BIOS entry point,
 *  leaving the caller's argument registers available to the selected service.
 *  @par Globals
 *  No named globals are referenced.
 *  @par Control flow
 *  Jumps to address 0xB0 with selector 0x19 set in the delay slot.
 *  @par Calls
 *  No named functions are called; the BIOS vector receives control.
 *  @par Side effects
 *  Depends on the selected BIOS service; the stub sets registers t1 and t2.
 *  @par Callers
 *  Confirmed: func_80091614 and func_80091AA4.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_80091B80\n"
    ".ent func_80091B80\n"
    "func_80091B80:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x19\n"
    ".end func_80091B80\n"
    ".set reorder\n"
);
