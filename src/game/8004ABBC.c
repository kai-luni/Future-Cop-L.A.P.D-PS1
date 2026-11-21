// Categories: Game Engine > Object Pool | Resources > Memory Management
#include "game/types.h"

extern void *gNodePoolAllocation __asm__("D_8009F79C");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Frees the 199-node effect/attachment pool buffer D_8009F79C and clears the pointer.
 *
 * Passes D_8009F79C (set alongside free-list head D_8009F798 to the 0x44C0-byte pool allocation in func_8004AB4C) to func_800181D4, then zeroes D_8009F79C.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F79C - base pointer to the 0x44C0-byte effect-node pool block (0x588($gp)), freed and set to 0.
 *
 * @par Control flow
 * Straight-line call to func_800181D4(D_8009F79C), store of 0 to D_8009F79C, and return.
 *
 * @par Calls
 * func_800181D4 - releases the 0x44C0-byte pool block.
 *
 * @par Side effects
 * Frees the D_8009F79C pool allocation and writes D_8009F79C = 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D48, between-mission subsystem reset) / Likely: none / Unknown: none.
 */
void func_8004ABBC(void)
{
    gFreeBlock(gNodePoolAllocation);
    gNodePoolAllocation = 0;
}
