// Categories: Game Engine > Frame | Platform > Scratchpad
#include "game/types.h"

extern u32 gHookInstallFlag __asm__("D_8009FAD8");
extern void gUpdateCameraAnchor(void) __asm__("func_80066C9C");

/**
 * @brief Scratchpad-stack trampoline for the camera anchor update
 *        func_80066C9C.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Pushes $ra, saves $sp to D_8009FAD8, sets $sp = 0x1F8003F8 (the fast
 * 1 KB scratchpad), calls func_80066C9C, then restores $sp and $ra. Same
 * pattern as func_80026D5C/func_80026E1C/func_8003F530; the shared save
 * slot means these trampolines cannot nest.
 *
 * @par Likely role
 * Runs the per-view camera anchor projection on the scratchpad stack for
 * speed: func_80066C9C writes the D_800A3190 position pairs the scene
 * traversal reads back.
 *
 * @par Callers
 * func_80027510.
 */
void func_80026E5C(void)
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
