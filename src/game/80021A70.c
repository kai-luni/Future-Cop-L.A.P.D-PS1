// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct EventFlags {
    u8 pad[0x14];
    u32 pending;
    u32 acknowledged;
};

extern struct EventFlags gSoundSystem __asm__("D_800A07A8");

void gQueueSoundVoiceOnMask(u32 flags) __asm__("func_80021A70");

/**
 * @brief Adds voice bits to the sound system's pending key-on mask.
 *
 * @param flags Mask of voices to mark for key-on.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads word +0x14, ORs it with $a0, and stores the result in the return delay
 * slot.
 *
 * @par Globals
 * Reads and updates gSoundSystem at +0x14.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021F98, func_800221E4.
 *
 * @par Role and path
 * Voice startup callers accumulate bits for the SPU key-on service at
 * func_80021E1C.
 *
 * @par Unknown
 * The caller selects the voice bits to set.
 */
void gQueueSoundVoiceOnMask(u32 flags)
{
    gSoundSystem.pending |= flags;
}
