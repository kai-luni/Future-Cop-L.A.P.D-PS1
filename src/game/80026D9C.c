// Categories: Game Engine > Frame | Platform > Scratchpad | Gameplay > Entity Script
#include "game/types.h"

extern u32 gHookInstallFlag __asm__("D_8009FAD8");
extern void func_80049564(s32 elapsed);

/**
 * @brief Runs the script-timer tick pass on the PS1 scratchpad stack.
 *
 * Confirmed: inline assembly saves $ra, stores $sp in D_8009FAD8, switches $sp to scratchpad 0x1F8003FC, forwards $a0 (elapsed) to func_80049564, and restores $sp and $ra.
 * Category follows the 0x1F8003FC scratchpad stack switch, per-frame invocation when unpaused, and ScriptTimer/bytecode evaluation in func_80049564.
 * Likely: non-nestable frame trampoline optimizing timer-loop stack access; Unknown: why 0x1F8003FC leaves one word below scratchpad top.
 *
 * @param elapsed Frame elapsed tick delta passed through in $a0 to func_80049564.
 *
 * @par Globals
 * D_8009FAD8 - saved main-RAM stack pointer while scratchpad stack is active; D_8009F770/D_8009F774 - script timer array/count updated by func_80049564.
 * @par Control flow
 * Linear inline-assembly trampoline: preserve $ra and $sp, switch $sp to 0x1F8003FC, call func_80049564, restore $sp and $ra, return.
 * @par Calls
 * func_80049564 - decrements D_8009F770 ScriptTimer countdowns by elapsed and evaluates trigger/action scripts via func_80069D68.
 * @par Side effects
 * Overwrites D_8009FAD8, uses scratchpad stack below 0x1F8003FC, and updates script timer state and VM side effects via func_80049564.
 * @par Callers
 * Confirmed: func_80027510 (0x80027CA0 in asm/us/main.s, with D_8009F9FC); func_800271A4 (0x80027494 in asm/us/main.s, with 0x7E8($gp)) / Likely: none / Unknown: none.
 */
void func_80026D9C(s32 elapsed)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3FC\n\t"
        "jal func_80049564\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
