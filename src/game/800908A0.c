// Categories: Platform > CD-ROM | Resources > File Access
#include "game/types.h"

extern s32 gCdSavedCallbackPtr[] __asm__("D_8009CBF8");
extern s32 gSetCdSyncCallback(s32) __asm__("func_8008E194");
void gCdPauseCallback(void) __asm__("func_800908A0");


/**
 * @brief CD-ROM pause completion callback that restores the prior driver handler and clears busy.
 *
 * Registered via func_8008E194 before issuing CdlPause (command 9); restores the saved callback
 * word at D_8009CBF8 into D_8009C8A8 via func_8008E194 and clears D_8009CBD0.busy (+0x24).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009CBF8 - saved callback word inside D_8009CBD0 (+0x28); D_8009CBD0 - CD read state (+0x24 busy cleared via p[-1]).
 *
 * @par Control flow
 * Straight-line call to func_8008E194(*D_8009CBF8) followed by storing 0 at (D_8009CBF8 - 0x28)[9].
 *
 * @par Calls
 * func_8008E194 - restores the previous primary CD-ROM callback word in D_8009C8A8.
 *
 * @par Side effects
 * Restores D_8009C8A8 and writes 0 to D_8009CBF8[-1] (D_8009CBD0 + 0x24 busy flag).
 *
 * @par Callers
 * Confirmed: none direct; address taken as D_800908A0 via func_8008E194 in func_800908D4 (0x80090AF4), func_80090B48 (0x80090BD8), and func_80090E00 (0x80090F1C) / Likely: invoked by CD driver callback dispatch on CdlPause completion / Unknown: none.
 *
 * @par Role and path
 * Acts as the CD-ROM pause completion callback installed via func_8008E194
 * before issuing CdlPause in func_800908D4, func_80090B48, and func_80090E00.
 * It restores the saved callback *D_8009CBF8 into D_8009C8A8 via
 * func_8008E194 and clears the busy flag at D_8009CBD0 + 0x24.
 */
void gCdPauseCallback(void)
{
    register s32 *p __asm__("$16");
    p = gCdSavedCallbackPtr;
    __asm__ volatile("" : "+r"(p));
    gSetCdSyncCallback(*p);
    p -= 10;
    __asm__("" : "+r"(p));
    p[9] = 0;
}
