// Categories: Game Engine > Front End
#include "game/types.h"

extern s8 *gFontBankTable __asm__("D_8009F468");
extern void gSelectEnabledMenuItem(s32 arg0, s32 arg1, s32 arg2) __asm__("func_800787D8");

/**
 * @brief Re-selects an enabled group-0 item starting at the saved index.
 *
 * Passes the active menu's group-0 selection as the starting index, with
 * forward direction 1 and group 0.
 * @par Globals
 * D_8009F468 - active menu descriptor and group-0 selection.
 * @par Control flow
 * None.
 * @par Calls
 * gSelectEnabledMenuItem (func_800787D8) - selects an enabled menu item.
 * @par Side effects
 * May update the active menu's group-0 selection.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
void gReselectEnabledItem(void) __asm__("func_80082020");

void gReselectEnabledItem(void)
{
    gSelectEnabledMenuItem((s32)gFontBankTable[0], 0x1, 0x0);
}
