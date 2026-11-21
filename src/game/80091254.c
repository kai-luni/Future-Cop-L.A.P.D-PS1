// Categories: Game Engine > State Access
#include "game/types.h"

extern s32 gCdUseDmaFlag __asm__("D_8009C814");

/**
 *  @brief Replaces a signed global value and returns its previous value.
 *
 *  The assembly reads the old value and stores the incoming argument
 *  in the return jump's delay slot.
 *  @param arg0 New value supplied in the first argument register.
 *  @return Previous value of D_8009C814.
 *  @par Globals
 *  D_8009C814 is read and then overwritten.
 *  @par Control flow
 *  Returns after the unconditional store in the delay slot.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates D_8009C814.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Hand-written assembly stub that loads the current 32-bit word from
 * D_8009C814 into $v0, returns to $ra, and writes $a0 into D_8009C814 in the
 * branch delay slot. No callers appear in src/, and D_8009C814 is bound as
 * gCdUseDmaFlag while being tested for bit 0 DMA mode in func_800908D4
 * through func_80090FA0.
 */
void gSetCdUseDmaFlag(void) __asm__("func_80091254");

__asm__(
    ".text\n\t"
    ".set\tnoreorder\n\t"
    ".set\tnoat\n\t"
    ".globl\tfunc_80091254\n\t"
    ".ent\tfunc_80091254\n"
    "func_80091254:\n\t"
    "lui\t$2, %hi(D_8009C814)\n\t"
    "lw\t$2, %lo(D_8009C814)($2)\n\t"
    "lui\t$1, %hi(D_8009C814)\n\t"
    "jr\t$31\n\t"
    "sw\t$4, %lo(D_8009C814)($1)\n\t"
    ".set\tat\n\t"
    ".set\treorder\n\t"
    ".end\tfunc_80091254\n"
);
