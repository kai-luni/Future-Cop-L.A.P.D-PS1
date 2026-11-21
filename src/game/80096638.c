void gResetDispatchLoopGuard(void) __asm__("func_80096638");

// Categories: Game Engine > Dispatch
/**
 *  @brief Checks a global flag before a disabled indirect-call loop.
 *
 *  The assembly contains a table-dispatch loop, but initializes its count
 *  to zero, so no table entries are read or called in this build.
 *  @par Globals
 *  D_8009EA00 is read as the dispatch enable flag.
 *  @par Control flow
 *  A zero flag exits immediately; otherwise the zero count also exits.
 *  @par Calls
 *  None are reached; the assembly contains an indirect call in the loop.
 *  @par Side effects
 *  No persistent state is modified by the reachable instructions.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void gResetDispatchLoopGuard(void)
{
    __asm__ volatile (
        ".set noat\n"
        ".set noreorder\n"
        "lui   $8, %hi(D_8009EA00)\n"
        "lw    $8, %lo(D_8009EA00)($8)\n"
        "addiu $29, $29, -16\n"
        "sw    $16, 4($29)\n"
        "sw    $17, 8($29)\n"
        "sw    $31, 12($29)\n"
        "beq   $8, $0, 1f\n"
        "nop\n"
        "lui   $16, %hi(func_80010000)\n"
        "addiu $16, $16, %lo(func_80010000)\n"
        "lui   $17, 0\n"
        "addiu $17, $17, 0\n"
        "beq   $17, $0, 1f\n"
        "nop\n"
        "2:\n"
        "lw    $8, 0($16)\n"
        "addiu $16, $16, 4\n"
        "jalr  $8\n"
        "addiu $17, $17, -1\n"
        "bne   $17, $0, 2b\n"
        "nop\n"
        "1:\n"
        "lw    $31, 12($29)\n"
        "lw    $17, 8($29)\n"
        "lw    $16, 4($29)\n"
        "addiu $29, $29, 16\n"
        ".set reorder\n"
        ".set at\n"
    );
}
