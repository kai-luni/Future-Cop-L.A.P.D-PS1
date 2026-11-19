// Categories: Game Engine > Audio
#include "game/types.h"

extern void func_80021A70(u32 mask);

void gQueueSoundChannelStarts(s32 count, s32 *channels) __asm__("func_800221E4");

/**
 * @brief Queues a set of channel indices for activation.
 *
 * @param count Number of channel entries to inspect.
 * @param channels Pointer to the channel index array; -1 entries are skipped.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Scans four-byte indices, skips -1 and ORs each SLLV-derived bit into a0 before calling
 * func_80021A70, even for an empty mask.
 *
 * @par Globals
 * None directly. The callee ORs the mask into gSoundSystem +0x14, also addressed as
 * gSoundChannelEnableMask.
 *
 * @par Callers
 * Direct calls from func_800228F4, func_80023F08.
 *
 * @par Role and path
 * Stream startup batches its assigned channels into a single update.
 *
 * @par Unknown
 * The loop accepts any index other than the -1 sentinel.
 */
void gQueueSoundChannelStarts(s32 count, s32 *channels)
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
