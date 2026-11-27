// Categories: Game Engine > State
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");

extern void gFlushSoundChannelUpdates(void) __asm__("func_80021E1C");

void gFlushSoundChannelsIfCvkActive(void) __asm__("func_80023314");

/**
 * @brief Flushes pending sound-channel changes while CVK audio is active.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the enable byte at gp +0x58 and calls gFlushSoundChannelUpdates only when that byte is
 * nonzero.
 *
 * @par Globals
 * Reads gCvkSystemActive.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Registered as a screen-mode hook by gInitCvkStreamSubsystem and removed by gShutdownCvkSystem
 * through the retail alias gFlushSoundChannelsIfCvkActive.
 *
 * @par Unknown
 * The hook itself has no timing logic.
 */
void gFlushSoundChannelsIfCvkActive(void)
{
    if (gCvkSystemActive != 0) {
        gFlushSoundChannelUpdates();
    }
}
