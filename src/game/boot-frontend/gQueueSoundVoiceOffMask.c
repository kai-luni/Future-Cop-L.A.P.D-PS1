// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct FlagState {
    u8 pad_00[0x18];
    u32 flags;
};

extern struct FlagState gSoundSystem __asm__("D_800A07A8");

void gQueueSoundVoiceOffMask(u32 flags) __asm__("func_80021AB4");

/**
 * @brief Adds voice bits to the sound system's pending key-off mask.
 *
 * @param flags Mask of voices to mark for key-off.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads word +0x18, ORs it with $a0, and stores the result in the return delay
 * slot.
 *
 * @par Globals
 * Reads and updates gSoundSystem at +0x18.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021F3C.
 *
 * @par Role and path
 * The voice reset path constructs a mask of configured channels and sends it
 * here before servicing sound state.
 *
 * @par Unknown
 * Voice selection is performed by the caller.
 */
void gQueueSoundVoiceOffMask(u32 flags)
{
    gSoundSystem.flags |= flags;
}
