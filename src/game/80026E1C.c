// Categories: Game Engine > Frame | Platform > Scratchpad
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void gDrawViewLayers(void) __asm__("func_80066E98");
void gRunRegionPass(void) __asm__("func_80026E1C");


/**
 * @brief Runs the region drawing pass func_80066E98 on a scratchpad stack.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Inline-asm trampoline: push $ra, save $sp to D_8009FAD8, set $sp to
 * 0x1F8003F8 (scratchpad), jal func_80066E98, restore $sp and $ra. Same
 * shared save slot as func_80026D5C / func_8003F530 (no nesting).
 *
 * @par Callers
 * func_80027510.
 *
 * @par Environment path
 * Runs the environment pass func_80066E98 on the scratchpad stack; the frame
 * driver func_80027510 calls it once per frame.
 */
void gRunRegionPass(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F8\n\t"
        "jal func_80066E98\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
