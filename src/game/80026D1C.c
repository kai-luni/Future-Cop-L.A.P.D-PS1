// Categories: Game Engine > Frame | Platform > Scratchpad | Gameplay > Entity Update
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void func_8004EC74(void);

/**
 * @brief Runs the per-frame entity message-0xD pass on the PS1 scratchpad stack.
 *
 * Confirmed: inline assembly saves $ra, stores $sp in D_8009FAD8, switches $sp to scratchpad 0x1F8003F8, calls func_8004EC74, and restores $sp and $ra.
 * Category follows the 0x1F8003F8 scratchpad stack switch and per-frame dispatch in func_80027510/func_800271A4 to update entities via func_8004EC74.
 * Likely: non-nestable speed optimization keeping entity-pass locals in 1 KB scratchpad RAM; Unknown: why 0x1F8003F8 is used instead of 0x1F800400.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FAD8 - temporary saved main-RAM stack pointer while scratchpad stack is active; D_800A8F90 - entity table read by callee func_8004EC74.
 * @par Control flow
 * Linear inline-assembly trampoline: save $ra and $sp, switch $sp, call func_8004EC74, restore $sp and $ra, return.
 * @par Calls
 * func_8004EC74 - walks D_800A8F90 and dispatches update(0xD, entity, 0, 0) for entities with id > 0 and unk88 != 0.
 * @par Side effects
 * Overwrites D_8009FAD8, uses scratchpad stack below 0x1F8003F8, and triggers entity updates and unk88/unk90 writes in func_8004EC74.
 * @par Callers
 * Confirmed: func_80027510 (0x80027C88 in asm/us/main.s, when !(flags & 0x20)); func_800271A4 (0x8002747C in asm/us/main.s, when !(flags & 0x20)) / Likely: none / Unknown: none.
 */
void func_80026D1C(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F8\n\t"
        "jal func_8004EC74\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
