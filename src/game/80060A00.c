// Categories: Game Engine > Game State > Reset | Game Engine > Object Setup
#include "game/types.h"

extern s32 gDefinitionEntryCount __asm__("D_8009F7F8");
void gResetDefinitionEntryCount(void) __asm__("func_80060A00");


/**
 * @brief Clears the 0x30-byte definition table entry count D_8009F7F8 during mission reset.
 *
 * Stores 0 into D_8009F7F8 in the between-missions reset path func_80024D14, mirroring the boot reset func_800609F4 before definition entries are loaded into D_800A2300. Category Game Engine > Game State > Reset | Game Engine > Object Setup follows the mission reset caller and definition-table counter global.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F7F8 - active entry count for the 0x30-byte definition table D_800A2300, reset to 0.
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
 * Confirmed: func_80024D14 (0x80024DE0 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void gResetDefinitionEntryCount(void)
{
    gDefinitionEntryCount = 0;
}
