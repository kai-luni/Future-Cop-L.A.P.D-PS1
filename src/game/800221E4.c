// Categories: Game Engine > Audio
#include "game/types.h"

extern void func_80021A70(u32 mask);

/**
 *  @brief Builds a channel bitmask and passes it to the channel operation.
 *
 *  Each channel index other than -1 contributes its corresponding bit to the
 *  mask. Duplicate indices contribute the same bit only once.
 *  @param count Number of channel entries to inspect.
 *  @param channels Pointer to the channel index array; -1 entries are skipped.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  A loop builds the mask, then the function is called even if the mask is zero.
 *  @par Calls
 *  func_80021A70 receives the completed mask.
 *  @par Side effects
 *  Any channel state changes are performed by func_80021A70.
 *  @par Callers
 *  Confirmed: func_800228F4.
 */
void func_800221E4(s32 count, s32 *channels)
{
    u32 mask;
    s32 i;

    mask = 0;
    for (i = 0; i < count; i++, channels++) {
        if (*channels != -1) {
            mask |= 1 << *channels;
        }
    }
    func_80021A70(mask);
}
