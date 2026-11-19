// Categories: Game Engine > Frame | Platform > Scratchpad
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void func_80062FB4(void);

void gRunHudPassOnScratchpad(void) __asm__("func_80026D5C");

/**
 * @brief Runs the viewport and HUD pass using the scratchpad stack.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Saves ra and the main stack pointer, sets sp to 0x1F800400 and calls func_80062FB4. Loads the
 * saved sp and restores the return address afterward.
 *
 * @par Globals
 * Writes and reads gSavedStackPointer.
 *
 * @par Callers
 * Direct calls from gRunSoloSessionFrame (func_800271A4), gRunSessionFrame (func_80027510).
 *
 * @par Role and path
 * The solo and shared session frames use this trampoline for viewport and HUD work.
 *
 * @par Unknown
 * The shared save word supplies one level of stack switching.
 */
void gRunHudPassOnScratchpad(void)
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
