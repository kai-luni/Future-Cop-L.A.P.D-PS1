// Categories: Game Engine > Stack Trampoline | Gameplay > Entity Update
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void gUpdatePlayerFormAndMotion(void) __asm__("func_80055988");

/**
 * @brief Runs func_80055988 with $sp moved to the top of the scratchpad, then
 *        restores the caller's stack pointer.
 *
 * Hand-written wrapper: it saves the caller's $sp in D_8009FAD8, points $sp at
 * 0x1F8003F4 (the last word of the 1 KB scratchpad at 0x1F800000), calls the
 * per-tick player-unit update func_80055988 and reloads $sp from the saved slot.
 * Only $sp, $at and $ra are touched, so the four argument registers and the
 * callee's return value pass through unchanged.
 *
 * @param void No C parameters. The caller's $a0-$a3 (object, elapsed, sub-step,
 *        output word) reach func_80055988 untouched.
 * @return The word func_80055988 leaves in $v0 - the flag/damage word the caller
 *         tests (bit 0 reports that a form change started or finished).
 *
 * @par Globals
 * D_8009FAD8 - 32-bit slot holding the caller's $sp across the switch; cleared
 * by the device-layer init func_8001E4E4 and shared with the identical wrapper
 * func_80026D1C (top 0x1F8003F8).
 *
 * @par Control flow
 * One straight-line block: reserve four bytes, save $ra and $sp, switch $sp,
 * call, reload $sp, pop $ra, return. No branches and no loops.
 *
 * @par Calls
 * func_80055988 - the only callee; the transformable unit's per-tick state
 * machine, which uses a 0x38-byte frame.
 *
 * @par Side effects
 * Writes D_8009FAD8 and $sp. All gameplay state changes come from the callee.
 *
 * @par Callers
 * Confirmed: func_800588D4 (jal at 0x80058AD8, damage branch of the unit's
 * message 6). Unknown: no other site in the call graph.
 */
void func_800586D4(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F4\n\t"
        "jal func_80055988\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
