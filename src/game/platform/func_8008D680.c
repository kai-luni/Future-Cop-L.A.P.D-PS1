// Categories: BIOS > Vector Dispatch
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches BIOS B0 service 0x4F.
 *
 *  This assembly stub transfers control to the BIOS B0 vector with
 *  service selector 0x4F in $t1. Its argument and result contract is
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
 *  Confirmed: func_8001F394, func_8001F818, func_8001F968,
 *  func_8001FAB8.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D680\n"
    ".ent func_8008D680\n"
    "func_8008D680:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x4F\n"
    ".end func_8008D680\n"
    ".set reorder\n"
);
