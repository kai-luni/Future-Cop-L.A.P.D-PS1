// Categories: Game Engine > Front End
#include "game/types.h"

extern s8 *gFontBankTable __asm__("D_8009F468");
extern void func_800787D8(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Re-applies the front-end mode stored in D_8009F468.
 *
 * The saved mode byte (signed) is passed to func_800787D8 together with the
 * override flag 1 and no extra argument, so the screen returns to whatever mode
 * was recorded before the menu change.
 * @par Globals
 * D_8009F468 - saved front-end mode byte.
 * @par Control flow
 * None.
 * @par Calls
 * func_800787D8 - screen mode setter.
 * @par Side effects
 * Changes the front-end presentation mode.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
void func_80082020(void)
{
    func_800787D8((s32)gFontBankTable[0], 0x1, 0x0);
}
