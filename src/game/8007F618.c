// Categories: Game Engine > Front End | Platform > Audio
#include "game/types.h"

extern u8 gPauseFlag __asm__("D_8009F3E8");

extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_8007F5D0(void);

/**
 * @brief Leaves the pause menu: plays the cue, clears the flag and restores the screen.
 *
 * Sound 0x49 is started, the pause flag D_8009F3E8 is cleared and func_8007F5D0
 * rebuilds the screen state behind the menu.
 * @par Globals
 * D_8009F3E8 - pause flag.
 * @par Control flow
 * Two calls around one store.
 * @par Calls
 * func_80046C84 - sound cue. func_8007F5D0 - screen restore.
 * @par Side effects
 * Plays a sound and resumes the game screen.
 * @par Callers
 * Unknown: no direct edge; reached from the menu input handler.
 */
void func_8007F618(void)
{
    func_80046C84(0x49, 0);
    gPauseFlag = 0;
    func_8007F5D0();
}
