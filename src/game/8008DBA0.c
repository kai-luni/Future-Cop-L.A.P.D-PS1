// Categories: BIOS > System Calls
/* Handwritten function: BIOS vector call, transcribed from the retail listing. */
/**
 * @brief Handwritten BIOS A0:0x44 stub (FlushCache).
 *
 * The PS1 cache is not coherent, so every block that was patched by the CPU has
 * to be flushed before the DMA engine reads it. The stub tail-jumps into the A0
 * vector with index 0x44.
 * @par Globals
 * None.
 * @par Control flow
 * Unconditional tail jump into the A0 vector.
 * @par Calls
 * None; BIOS A0:0x44 flushes the instruction cache.
 * @par Side effects
 * Flushes the I-cache for the patched region.
 * @par Callers
 * Confirmed: func_8008DB28, func_8008DCE8, func_8008DD7C (CD base block patches).
 */
__asm__(
    ".set noreorder\n"
    ".globl func_8008DBA0\n"
    ".ent func_8008DBA0\n"
    "func_8008DBA0:\n"
    "addiu     $t2, $zero, 0xA0\n"
    "jr        $t2\n"
    "addiu     $t1, $zero, 0x44\n"
    ".end func_8008DBA0\n"
    ".set reorder\n"
);
