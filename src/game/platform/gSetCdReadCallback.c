// Categories: Platform > CD-ROM | Platform > Interrupts
#include "game/types.h"

extern s32 gCdReadCallback __asm__("D_8009C810");

/**
 * @brief Installs a CD-ROM read completion callback in D_8009C810 and returns the previous one.
 *
 * Assembly stub matching Sony libcd CdReadCallback: loads the current D_8009C810 function pointer into $v0, jumps to $ra, and writes the new pointer from $a0 into D_8009C810 in the delay slot. Invoked on CD sector/DMA3 completion by func_800908D4 and func_80090B48.
 *
 * @param callback ($a0) New completion callback pointer to install in D_8009C810.
 * @return Previous D_8009C810 callback pointer.
 *
 * @par Globals
 * D_8009C810 - active CD read completion callback pointer (initialized to D_8008DFBC in func_8008DEA0).
 *
 * @par Control flow
 * Straight-line read, branch to $ra, and branch-delay store; no loops or conditions.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites D_8009C810 with $a0.
 *
 * @par Callers
 * Confirmed: gMovieStreamReaderInit (0x80089834, saves and clears callback on stream start), gMovieStreamReaderClose (0x800898A8 in asm/us/main.s, restores saved callback on stream stop) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Hand-written assembly stub matching Sony libcd CdReadCallback that loads
 * the current callback pointer from D_8009C810 into $v0, returns to $ra, and
 * stores the new callback pointer from $a0 into D_8009C810 in the branch
 * delay slot. D_8009C810 is invoked on CD sector/DMA3 completion by
 * func_800908D4 and func_80090B48; called by gMovieStreamReaderInit and
 * gMovieStreamReaderClose.
 */
void gSetCdReadCallback(void) __asm__("func_80091240");

__asm__(
    ".text\n\t"
    ".set\tnoreorder\n\t"
    ".set\tnoat\n\t"
    ".globl\tfunc_80091240\n\t"
    ".ent\tfunc_80091240\n"
    "func_80091240:\n\t"
    "lui\t$2, %hi(D_8009C810)\n\t"
    "lw\t$2, %lo(D_8009C810)($2)\n\t"
    "lui\t$1, %hi(D_8009C810)\n\t"
    "jr\t$31\n\t"
    "sw\t$4, %lo(D_8009C810)($1)\n\t"
    ".set\tat\n\t"
    ".set\treorder\n\t"
    ".end\tfunc_80091240\n"
);
