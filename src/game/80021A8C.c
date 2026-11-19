// Categories: Game Engine > Event Flags
#include "../../include/game/types.h"

struct EventFlags {
    u8 pad[0x14];
    u32 pending;
    u32 acknowledged;
};

extern struct EventFlags gSoundSystem __asm__("D_800A07A8");

void gMoveSoundVoiceMaskToOff(u32 flags) __asm__("func_80021A8C");

/**
 * @brief Moves selected voice bits out of the pending key-on mask and into the
 * key-off mask.
 *
 * @param flags Mask of voices to move from key-on to key-off.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Inverts the argument with NOR, ANDs that with word +0x14, and ORs the
 * original argument into word +0x18. Stores both words.
 *
 * @par Globals
 * Reads and updates gSoundSystem at +0x14 and +0x18.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_8002238C.
 *
 * @par Role and path
 * The voice stop path cancels pending starts and queues the voices for
 * key-off.
 *
 * @par Unknown
 * When the manager flushes these masks depends on the audio update path.
 */
void gMoveSoundVoiceMaskToOff(u32 flags)
{
    gSoundSystem.pending &= ~flags;
    gSoundSystem.acknowledged |= flags;
}
