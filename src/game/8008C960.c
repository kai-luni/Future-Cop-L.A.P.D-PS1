// Categories: BIOS > System Calls | Platform > Interrupts
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Invokes PS1 BIOS OpenEvent (B0:08) to allocate a kernel event descriptor.
 *
 * Handwritten BIOS vector trampoline setting $t2 = 0xB0 and $t1 = 0x8. Used by memory-card and SPU DMA setup to register interrupt callback events.
 *
 * @param (implicit) $a0 event class, $a1 event spec, $a2 event mode, $a3 callback handler.
 * @return BIOS event descriptor handle in $v0 (-1 on failure).
 *
 * @par Globals
 * None directly; modifies PS1 BIOS kernel event table.
 *
 * @par Control flow
 * Straight-line trampoline: load $t2 = 0xB0, jr $t2 with delay-slot $t1 = 0x8.
 *
 * @par Calls
 * None (jumps directly to PS1 BIOS B0 vector).
 *
 * @par Side effects
 * Allocates a kernel event slot and binds its callback in BIOS RAM.
 *
 * @par Callers
 * Confirmed: func_80020120 (0x8002015C..0x80020274), gSpuInitDmaEvent (0x80096E80).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008C960\n"
    ".ent func_8008C960\n"
    "func_8008C960:\n"
    "addiu     $t2, $zero, 0xB0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x8\n"
    ".end func_8008C960\n"
    ".set reorder\n"
);
