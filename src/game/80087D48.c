// Categories: Graphics > Video Layer | Resources > Memory Management
#include "game/types.h"

extern u32 gMovieDisplayMode __asm__("D_8009F4DC");
extern void *gMovieDecodeBufferBlock __asm__("D_8009F4E4");
extern void *gMovieSliceContextStorage __asm__("D_8009F4E8");
extern void *gMovieStreamBuffer __asm__("D_8009F4EC");
extern void *gMovieAudioBlock __asm__("D_8009F4F0");
extern void *gMovieSpuBufferA __asm__("D_8009F4F4");
extern void *gMovieSpuBufferB __asm__("D_8009F4F8");
extern void *gMoviePrimaryVlcBuffer __asm__("D_8009F4FC");
extern void *gMovieSecondaryVlcBuffer __asm__("D_8009F500");
extern u8 gLayerOpenResult __asm__("D_8009F504");

extern void gMovieStopAndRestoreCallbacks(void) __asm__("func_80085A24");
extern void gResetMovieFrameBuffers(s32, s32) __asm__("func_80085AC4");
extern s32 gGetMovieActiveBufferIndex(void) __asm__("func_800858E0");
extern void gSelectFadeLayer(u8, u8) __asm__("func_8001C088");
extern void gVSyncWait(s32) __asm__("func_80091270");
extern s32 func_8001BFE4(void);
extern void gClaimSpuEntry(void *) __asm__("func_80098708");
extern void gMovieFreeDecodeBuffer(void *) __asm__("func_800879BC");

/**
 * @brief Closes the video/image layer and releases everything gOpenVideoLayer()
 *        allocated.
 *
 * @par Confirmed flow
 * 1. gMovieStopAndRestoreCallbacks() stops the decoder.
 * 2. Unless the flag bit 0x20 is set in gMovieDisplayMode: func_80085AC4(1, 0),
 *    func_8001C088(func_800858E0(), 0) (disable the fade layer that the video
 *    used), func_80091270(0) and func_8001BFE4() restore the display.
 * 3. Then each of the eight buffers is released: gMovieSpuBufferB and gMovieSpuBufferA
 *    with func_80098708(), gMovieAudioBlock freely, gMovieStreamBuffer only when flag bit 0
 *    is clear, then D_8009F4E8, D_8009F500, D_8009F4FC, D_8009F4E4 with
 *    gMovieFreeDecodeBuffer().
 * 4. All pointers and D_8009F504 are cleared, so the layer is "closed".
 *
 * @par Globals
 * gMovieDisplayMode (mode flags), D_8009F4E4..D_8009F500 (the eight buffers),
 * D_8009F504 (open flag).
 *
 * @par Callers
 * gDrawImageResource (after a one-shot draw), gOpenVideoLayer (failure path) and the
 * unit at 0x80087EA4.
 */
void gCloseVideoLayer(void) __asm__("func_80087D48");

void gCloseVideoLayer(void)
{
    gMovieStopAndRestoreCallbacks();

    if (!(gMovieDisplayMode & 0x20)) {
        gResetMovieFrameBuffers(1, 0);
        gSelectFadeLayer((u8)gGetMovieActiveBufferIndex(), 0);
        gVSyncWait(0);
        func_8001BFE4();
    }

    if (gMovieSpuBufferB != 0) {
        gClaimSpuEntry(gMovieSpuBufferB);
    }
    if (gMovieSpuBufferA != 0) {
        gClaimSpuEntry(gMovieSpuBufferA);
    }
    if (gMovieAudioBlock != 0) {
        gMovieFreeDecodeBuffer(gMovieAudioBlock);
    }
    if (!(gMovieDisplayMode & 1)) {
        if (gMovieStreamBuffer != 0) {
            gMovieFreeDecodeBuffer(gMovieStreamBuffer);
        }
    }
    if (gMovieSliceContextStorage != 0) {
        gMovieFreeDecodeBuffer(gMovieSliceContextStorage);
    }
    if (gMovieSecondaryVlcBuffer != 0) {
        gMovieFreeDecodeBuffer(gMovieSecondaryVlcBuffer);
    }
    if (gMoviePrimaryVlcBuffer != 0) {
        gMovieFreeDecodeBuffer(gMoviePrimaryVlcBuffer);
    }
    if (gMovieDecodeBufferBlock != 0) {
        gMovieFreeDecodeBuffer(gMovieDecodeBufferBlock);
    }

    gMovieDecodeBufferBlock = 0;
    gMovieSliceContextStorage = 0;
    gMovieStreamBuffer = 0;
    gMovieAudioBlock = 0;
    gMovieSpuBufferA = 0;
    gMovieSpuBufferB = 0;
    gMoviePrimaryVlcBuffer = 0;
    gMovieSecondaryVlcBuffer = 0;
    gLayerOpenResult = 0;
}
