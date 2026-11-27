// Categories: BIOS > Vector Dispatch
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 *  @brief Dispatches BIOS A0 service 0x2E.
 *
 *  This assembly stub transfers control to the BIOS A0 vector with
 *  service selector 0x2E in $t1. Its argument and result contract is
 *  not declared in the source.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Tail-jumps to 0xA0; the selector is set in the jump delay slot.
 *  @par Calls
 *  BIOS A0 vector: handles the selected service.
 *  @par Side effects
 *  Sets $t2 and $t1; further effects depend on the BIOS service.
 *  @par Callers
 *  Confirmed: func_8008CD3C.
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xA0 and $t1 = 0x2E to
 * invoke PS1 BIOS A0:2E (memchr), scanning up to $a2 bytes at $a0 for byte
 * $a1. Called by func_8008CD3C.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008D5C0\n"
    ".ent func_8008D5C0\n"
    "func_8008D5C0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x2E\n"
    ".end func_8008D5C0\n"
    ".set reorder\n"
);
