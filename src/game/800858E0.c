// Categories: Graphics > Video Layer > Playback Context | Resources > Movie Playback
#include "game/types.h"

struct UnkF4BC {
    u8 pad[0x114];
    s32 unk114;
};

extern struct UnkF4BC *D_8009F4BC;

/**
 * @brief Returns the active buffer index of the playback context.
 *
 * @return D_8009F4BC->unk114, the 0/1 selector of the double buffer pair.
 *
 * @par Confirmed
 * Single field read. func_80085AC4 uses the same field to pick the
 * inactive buffer pair when flipping.
 *
 * @par Callers
 * func_80087D48.
 */
s32 func_800858E0(void)
{
    return D_8009F4BC->unk114;
}
