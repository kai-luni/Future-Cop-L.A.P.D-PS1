// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct EventFlags {
    u8 pad[0x14];
    u32 pending;
    u32 acknowledged;
};

extern struct EventFlags gSoundSystem __asm__("D_800A07A8");

/**
 *  @brief Adds bits to the pending event flags.
 *
 *  ORs the supplied mask into the pending field of the shared event state.
 *  Existing pending bits remain set.
 *  @param flags Bit mask to add to the pending flags.
 *  @par Globals
 *  D_800A07A8.pending is read and updated.
 *  @par Control flow
 *  Performs one unconditional read-modify-write operation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the pending field of D_800A07A8.
 *  @par Callers
 *  Confirmed: func_80021F98, func_800221E4.
 */
void func_80021A70(u32 flags)
{
    gSoundSystem.pending |= flags;
}
