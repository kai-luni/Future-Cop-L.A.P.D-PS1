// Categories: Game Engine > Init | Game Engine > Object Setup
#include "game/types.h"

extern s32 gDefinitionEntryCount __asm__("D_8009F7F8");
void gInitDefinitionEntryCount(void) __asm__("func_800609F4");


/**
 * @brief Resets the 0x30-byte definition table entry count D_8009F7F8 during mission boot.
 *
 * Clears D_8009F7F8 (sw $zero, 0x5E4($gp)) before definition entries are loaded into D_800A2300 via func_80060838.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F7F8 - active entry count for definition table D_800A2300.
 *
 * @par Control flow
 * Straight-line store of 0 and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F7F8 = 0.
 *
 * @par Callers
 * Confirmed: func_80024A24 (0x80024C00, mission boot sequence) / Likely: none / Unknown: none.
 */
void gInitDefinitionEntryCount(void)
{
    gDefinitionEntryCount = 0;
}
