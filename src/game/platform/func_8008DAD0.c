// Categories: BIOS > Vector Dispatch
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches BIOS B0 service 0x13.
 *
 *  This assembly stub transfers control to the BIOS B0 vector with
 *  service selector 0x13 in $t1. Its argument and result contract is
 *  not declared in the source.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Tail-jumps to 0xB0; the selector is set in the jump delay slot.
 *  @par Calls
 *  BIOS B0 vector: handles the selected service.
 *  @par Side effects
 *  Sets $t2 and $t1; further effects depend on the BIOS service.
 *  @par Callers
 *  Confirmed: func_8008D8FC.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DAD0\n"
    ".ent func_8008DAD0\n"
    "func_8008DAD0:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x13\n"
    ".end func_8008DAD0\n"
    ".set reorder\n"
);
