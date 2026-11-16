// Categories: Platform > Sound > Voices
#include "game/types.h"

extern void func_80021A8C(u32 mask);

/**
 * @brief Releases or acknowledges a single sound channel by bitmask.
 *
 * Converts channel index to bitmask (1 << channel) and passes it to func_80021A8C, which clears that bit in D_800A07A8.pending (+0x14) and sets it in D_800A07A8.acknowledged (+0x18).
 *
 * @param channel Zero-based sound channel index (0..31).
 *
 * @par Globals
 * None directly; func_80021A8C updates D_800A07A8 (+0x14 pending, +0x18 acknowledged).
 *
 * @par Control flow
 * Computes 1 << channel and delegates to func_80021A8C.
 *
 * @par Calls
 * func_80021A8C - clears bitmask in pending and sets it in acknowledged inside D_800A07A8.
 *
 * @par Side effects
 * Updates D_800A07A8 pending/acknowledged channel bitmasks via func_80021A8C.
 *
 * @par Callers
 * Confirmed: func_80022240 (0x8002228C, clears voice slots by owner), func_800243F8 (0x80024404, stops channel from voice descriptor +0x0D) / Likely: none / Unknown: none.
 */
void func_8002238C(s32 channel)
{
    func_80021A8C(1 << channel);
}
