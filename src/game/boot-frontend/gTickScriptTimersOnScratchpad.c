// Categories: Game Engine > Frame | Platform > Scratchpad | Gameplay > Entity Script
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void gTickScriptTimers(s32 elapsed) __asm__("func_80049564");

void gTickScriptTimersOnScratchpad(s32 elapsed) __asm__("func_80026D9C");

/**
 * @brief Ticks the script timers on the scratchpad stack.
 *
 * @param elapsed Frame elapsed tick delta passed through in $a0 to func_80049564.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Saves ra and sp, switches sp to 0x1F8003FC and calls func_80049564 with the incoming a0
 * unchanged. Restores the main stack and return address afterward.
 *
 * @par Globals
 * Writes and reads gSavedStackPointer. The callee updates gScriptTimerTicks and reads the timer
 * count at D_8009F774.
 *
 * @par Callers
 * Direct calls from gRunSoloSessionFrame (func_800271A4), gRunSessionFrame (func_80027510).
 *
 * @par Role and path
 * The solo and shared session frames pass their elapsed frame delta into the script timer
 * update.
 *
 * @par Unknown
 * The reason for the four-byte offset below the scratchpad end is unresolved.
 */
void gTickScriptTimersOnScratchpad(s32 elapsed)
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
