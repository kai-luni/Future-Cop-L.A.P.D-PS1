// Categories: BIOS > System Calls | Platform > Interrupts
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Invokes PS1 BIOS EnableEvent (B0:0C) to arm a kernel event descriptor.
 *
 * Handwritten BIOS vector trampoline setting $t2 = 0xB0 and $t1 = 0xC. Used immediately after func_8008C960 to enable registered memory-card and SPU DMA events.
 *
 * @param (implicit) $a0 event handle returned by func_8008C960.
 * @return Non-zero on success, 0 on failure (left in $v0 by BIOS B0:0C).
 *
 * @par Globals
 * None directly; updates PS1 BIOS kernel event state.
 *
 * @par Control flow
 * Straight-line trampoline: load $t2 = 0xB0, jr $t2 with delay-slot $t1 = 0xC.
 *
 * @par Calls
 * None (jumps directly to PS1 BIOS B0 vector).
 *
 * @par Side effects
 * Enables event dispatch for the supplied BIOS event handle.
 *
 * @par Callers
 * Confirmed: func_80020120 (0x8002028C..0x80020334), func_80096E40 (0x80096E90).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C980\n"
    ".ent func_8008C980\n"
    "func_8008C980:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0xC\n"
    ".end func_8008C980\n"
    ".set reorder\n"
);
