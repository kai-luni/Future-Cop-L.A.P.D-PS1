// Categories: Game Engine > Front-End > Map Screen | Platform > Scratchpad
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void gUpdateMapScreenView(void) __asm__("func_800267C8");
void gRunMapScreenOnScratch(void) __asm__("func_80026DDC");


/**
 * @brief Runs the map-screen row update on a stack in the scratchpad.
 *
 * Inline-assembly trampoline of the same family as func_80026D5C and func_80026E5C: it pushes the return address, stores the caller's stack pointer in D_8009FAD8, switches the stack to 0x1F8003F0 (top of the 1 KB scratchpad at 0x1F800000) and calls func_800267C8.
 *
 * @return Nothing; the four caller-supplied arguments pass through unchanged.
 *
 * @par Globals
 * D_8009FAD8 - saves the interrupted stack pointer; shared with the other trampolines of the family, so they must not nest.
 *
 * @par Control flow
 * Push, save, switch, call, restore the stack pointer, return - every step in inline assembly, no C-level branches.
 *
 * @par Calls
 * func_800267C8 - the per-slot front-end map-screen row update.
 *
 * @par Side effects
 * The scratchpad below 0x1F8003F0 is used as a stack and its previous content is not preserved; D_8009FAD8 is left holding a stale pointer. The row update runs on the hot split-screen path, which is why its frames live there.
 *
 * @par Callers
 * Confirmed: func_80027510 (0x8002790C, 0x80027B1C) with (game, slot, paused, menu mode).
 * Unknown: one call at 0x800273B0 from a routine that is not in the matching set.
 *
 * @par Environment path
 * Inline-assembly trampoline: it pushes $ra, saves $sp in D_8009FAD8, moves
 * the stack to 0x1F8003F0 at the top of the scratchpad, calls
 * gUpdateMapScreenView (func_800267C8) with the four argument registers
 * untouched, then restores the stack and $ra. gRunSessionFrame
 * (func_80027510) calls it at 80027510.c:247 and :286 for player 0/1 whose
 * view flag (bit 0/bit 1) is set, in place of the normal 3D gRunRegionPass,
 * and gRunSoloSessionFrame (func_800271A4) at 800271A4.c:187; so it runs
 * every frame while a player has the map screen open. The same D_8009FAD8
 * slot is used by the sibling trampolines func_80026D1C, func_80026D5C and
 * func_80026E5C, so these must not nest.
 */
void gRunMapScreenOnScratch(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F0\n\t"
        "jal func_800267C8\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
