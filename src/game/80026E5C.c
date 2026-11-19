// Categories: Game Engine > Frame | Platform > Scratchpad
#include "game/types.h"

extern u32 gSavedStackPointer __asm__("D_8009FAD8");
extern void gUpdateCameraAnchor(void) __asm__("func_80066C9C");

void gUpdateViewAnchorsOnScratchpad(void) __asm__("func_80026E5C");
/**
 * @brief Runs the per-view camera anchor update on the scratchpad stack.
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Saves ra and sp, calls gUpdateCameraAnchor with sp at 0x1F8003F8, then restores sp and ra.
 *
 * @par Globals
 * Writes and reads gSavedStackPointer. The callee updates gViewGroundAnchors.
 *
 * @par Callers
 * Direct calls from gRunSoloSessionFrame (func_800271A4), gRunSessionFrame (func_80027510).
 *
 * @par Role and path
 * Both session frame paths build view positions through this scratchpad trampoline.
 * @par Unknown
 * The reason for the eight-byte gap below the scratchpad end is unresolved.
 */
void gUpdateViewAnchorsOnScratchpad(void)
{
    __asm__ volatile(
        "addiu $sp, $sp, -0x4\n\t"
        "sw $ra, 0x0($sp)\n\t"
        "lui $at, %hi(D_8009FAD8)\n\t"
        "sw $sp, %lo(D_8009FAD8)($at)\n\t"
        "nop\n\t"
        "lui $sp, 0x1F80\n\t"
        "ori $sp, $sp, 0x3F8\n\t"
        "jal func_80066C9C\n\t"
        "lui $sp, %hi(D_8009FAD8)\n\t"
        "lw $sp, %lo(D_8009FAD8)($sp)\n\t"
        "nop\n\t"
        "lw $ra, 0x0($sp)\n\t"
        "addiu $sp, $sp, 0x4\n\t"
    );
}
