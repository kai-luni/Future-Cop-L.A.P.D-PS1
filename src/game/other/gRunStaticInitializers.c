void gRunStaticInitializers(void) __asm__("func_800965C8");

// Categories: Platform > Runtime Init | Utilities
/**
 * @brief One-shot initialiser: runs the constructor table once per program
 *        run and records that it happened.
 *
 * @return Nothing.
 *
 * @par Confirmed flow (the body is inline assembly, word-exact from retail)
 * 1. Loads D_8009EA00; if it is non-zero the function returns after restoring
 *    the saved registers (the guard).
 * 2. Stores 1 into D_8009EA00.
 * 3. Walks a table starting at func_80010000: load word, advance, @c jalr, and
 *    repeat $17 times. In this build $17 is loaded as 0 (lui/addiu 0), so the
 *    loop body never executes - the shipping executable has an empty
 *    constructor table and this call only sets the guard.
 *
 * @par Globals
 * D_8009EA00: "runtime already initialised" flag.
 *
 * @par Likely origin
 * The Sony crt0/compiler-runtime pattern for C++ static constructors or
 * __attribute__((constructor)) functions; the empty table suggests the
 * constructors were linked elsewhere or the driver was left unused.
 *
 * @par Callers
 * func_80076A2C, as the very first thing it does.
 */
void gRunStaticInitializers(void)
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
        "bne   $8, $0, 1f\n"
        "ori   $8, $0, 1\n"
        "lui   $1, %hi(D_8009EA00)\n"
        "sw    $8, %lo(D_8009EA00)($1)\n"
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
