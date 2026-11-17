// Categories: Game Engine > Init | Gameplay > Placement
#include "game/types.h"

extern s32 gPlacementSlotCount __asm__("D_8009F7B8");

/**
 * @brief Resets the placement-header slot count D_8009F7B8 to zero during mission boot.
 *
 * Clears D_8009F7B8 (sw $zero, 0x5A4($gp)) before loaded placement records populate D_800A2210 via func_800511E4.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F7B8 - running entry count for placement header array D_800A2210.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F7B8 = 0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C44, mission boot sequence) / Likely: none / Unknown: none.
 */
void func_8005130C(void)
{
    gPlacementSlotCount = 0;
}
