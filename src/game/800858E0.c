// Categories: Graphics > Video Layer > Playback Context | Resources > Movie Playback
#include "game/types.h"

struct UnkF4BC {
    u8 pad[0x114];
    s32 unk114;
};

extern struct UnkF4BC *gMovieSliceContext __asm__("D_8009F4BC");
s32 gGetMovieActiveBufferIndex(void) __asm__("func_800858E0");


/**
 * @brief Returns the active buffer index of the playback context.
 *
 * @return gMovieSliceContext->unk114, the 0/1 selector of the double buffer pair.
 *
 * @par Confirmed
 * Single field read. func_80085AC4 uses the same field to pick the
 * inactive buffer pair when flipping.
 *
 * @par Callers
 * gCloseVideoLayer.
 */
s32 gGetMovieActiveBufferIndex(void)
{
    return gMovieSliceContext->unk114;
}
