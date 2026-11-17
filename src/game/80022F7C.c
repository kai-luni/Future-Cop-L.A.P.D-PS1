// Categories: Platform > Audio
#include "game/types.h"

extern s32 gCurrentChannel __asm__("D_800A09F0");

extern void func_80098A78(s32 mode);

/**
 * @brief Marks the audio stream state as reset and waits for the SPU flag.
 *
 * D_800A09F0 is set to -1 (the "no channel" marker) and func_80098A78(0) then
 * spins until the transfer flag clears, so a following start cannot land on a
 * busy stream.
 * @par Globals
 * D_800A09F0 - channel/stream id, set to -1.
 * @par Control flow
 * None.
 * @par Calls
 * func_80098A78 - SPU flag wait.
 * @par Side effects
 * Writes the stream id and waits on hardware state.
 * @par Callers
 * Confirmed: func_80022F44, func_80022FA8 (same block).
 */
void func_80022F7C(void)
{
    gCurrentChannel = -1;
    func_80098A78(0);
}
