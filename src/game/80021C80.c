// Categories: Platform > Sound | Platform > Device Shutdown
#include "game/types.h"

extern void gQueueSoundChannelShutdown(void) __asm__("func_80021F3C");
extern void gSpuShutdownDmaEvent(void) __asm__("func_80097A48");
extern void func_800216D8(void);

void gShutdownSoundSystem(void) __asm__("func_80021C80");

/**
 * @brief Shuts down registered sound channels, the SPU DMA event and sound bank storage.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gQueueSoundChannelShutdown, gSpuShutdownDmaEvent and func_800216D8, in that order.
 *
 * @par Globals
 * None directly. The callees flush gSoundSystem channel state and release its bank and channel
 * references.
 *
 * @par Callers
 * Direct calls from gConfigureSoundSystem (func_80021C20), gShutdownCvkSystem (func_80023478).
 *
 * @par Role and path
 * Runs on the sound-system teardown path and is the configuration wrapper's nonzero-result
 * cleanup.
 *
 * @par Unknown
 * The wrapper does not inspect teardown results.
 */
void gShutdownSoundSystem(void)
{
    gQueueSoundChannelShutdown();
    gSpuShutdownDmaEvent();
    func_800216D8();
}
