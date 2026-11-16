// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct EventFlags {
    u8 pad[0x14];
    u32 pending;
    u32 acknowledged;
};

extern struct EventFlags D_800A07A8;

/**
 *  @brief Acknowledges the specified event flags.
 *
 *  Clears the selected bits from the pending mask and sets them in the
 *  acknowledged mask, preserving all other bits in both fields.
 *  @param flags Bit mask of events to acknowledge.
 *  @par Globals
 *  D_800A07A8 holds the pending and acknowledged event masks.
 *  @par Control flow
 *  Updates both masks directly without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the pending and acknowledged fields of D_800A07A8.
 *  @par Callers
 *  Confirmed: func_8002238C.
 */
void func_80021A8C(u32 flags)
{
    D_800A07A8.pending &= ~flags;
    D_800A07A8.acknowledged |= flags;
}
