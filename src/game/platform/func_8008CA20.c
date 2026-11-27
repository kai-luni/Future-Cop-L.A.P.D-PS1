// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS C0:0x0A stub (change thread priority class).
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the C0 vector.
 * @par Calls
 * None; BIOS C0:0x0A performs the change.
 * @par Side effects
 * Alters the current thread's scheduling class.
 * @par Callers
 * Confirmed: func_80091270's neighbourhood (frame/timer bridge).
 *
 * @par Role and path
 * Handwritten MIPS BIOS trampoline setting $t2 = 0xC0 and $t1 = 0x0A to
 * invoke PS1 BIOS C0:0A (ChangeClearRCnt), configuring root counter
 * auto-clear behavior. Called by func_8008A5BC and the frame/timer bridge
 * around func_80091270.
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008CA20\n"
    ".ent func_8008CA20\n"
    "func_8008CA20:\n"
    "addiu     $t2, $zero, 0xC0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xA\n"
    ".end func_8008CA20\n"
    ".set reorder\n"
);
