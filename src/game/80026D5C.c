// Categories: Game Engine > Frame | Platform > Scratchpad
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void func_80062FB4(void);

/**
 * @brief Runs the viewport/HUD pass func_80062FB4 on a scratchpad stack.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Inline-asm trampoline: push $ra, save $sp to D_8009FAD8, set $sp to
 * 0x1F800400 (scratchpad), jal func_80062FB4, restore $sp from D_8009FAD8
 * and $ra. The save slot is shared with func_80026E1C and func_8003F530,
 * so the trampolines must not nest.
 *
 * @par Likely role
 * The scratchpad's fast 1 KB RAM serves as stack for the hot render path.
 *
 * @par Callers
 * func_80027510.
 */
void func_80026D5C(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x400\n\t"
        "jal func_80062FB4\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
