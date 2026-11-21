// Categories: Graphics > Video Layer > Playback Context | Resources > Movie Playback | Platform > GPU
#include "game/types.h"

typedef struct {
    u8 pad[0x114];
    s32 unk114;
    s32 unk118;
} Ctx85AC4;

extern Ctx85AC4 *gMovieSliceContext __asm__("D_8009F4BC");
extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");
extern u32 gStripStepWidth __asm__("D_8009F4C8");

extern void func_80092C8C(s32 arg0);
extern void gMdecInSync(s32 arg0) __asm__("func_80096090");
extern void gMdecOutSync(s32 arg0) __asm__("func_800960CC");
extern void gClearImage(s16 *buf, s32 a1, s32 a2, s32 a3) __asm__("func_80092E10");
extern void gVSyncWait(s32 arg0) __asm__("func_80091270");
extern void func_800932F0(void *ptr);
extern void gPutDispEnvCached(void *ptr) __asm__("func_800934BC");
extern void func_80092BF4(s32 arg0);
extern void func_80095E90(s32 arg0);
void gResetMovieFrameBuffers(u32 arg0, u32 arg1) __asm__("func_80085AC4");


/**
 * @brief Resets the playback context: stops GPU and DMA activity,
 *        optionally uploads two draw-environment blocks and flips the
 *        double buffer.
 *
 * @param arg0 Low byte nonzero: upload the two GPU data blocks.
 * @param arg1 Low byte nonzero: wait, flip to the other buffer set and
 *             flush the GPU command queue.
 * @return Nothing.
 *
 * @par Confirmed
 * Clears gMovieSliceContext->unk114/unk118, busy-waits until D_8009FA3C == 0,
 * then func_80092C8C(0), func_80096090(0), func_800960CC(0).
 * arg0 path: two func_80092E10 uploads of {0, 0, v, 0xF0} and
 * {0, 0x100, v, 0xF0} with v = ((D_8009F4C8 * 5) << 6) >> 1, each followed
 * by func_80092C8C(0). arg1 path: func_80091270(0), func_800932F0 and
 * func_800934BC on the buffer pair selected by (1 - unk114) (strides 92
 * and 20 from +0x34 / +0xEC), func_80092BF4(1). Finally func_80095E90(0)
 * (display option 0).
 *
 * @par Likely role
 * Start/stop hook of the movie layer: the two uploads set the draw
 * environment of both buffers, the flip exchanges the active pair.
 *
 * @par Callers
 * func_80085C4C, gCloseVideoLayer.
 */
void gResetMovieFrameBuffers(u32 arg0, u32 arg1)
{
    s16 buf[4];

    gMovieSliceContext->unk114 = 0;
    gMovieSliceContext->unk118 = 0;

    while (gFrameBarrier != 0) {
    }

    func_80092C8C(0);
    gMdecInSync(0);
    gMdecOutSync(0);

    if ((arg0 & 0xFF) != 0) {
        buf[0] = 0;
        buf[1] = 0;
        buf[3] = 0xF0;
        buf[2] = ((gStripStepWidth * 5) << 6) >> 1;
        gClearImage(buf, 0, 0, 0);
        func_80092C8C(0);

        buf[0] = 0;
        buf[1] = 0x100;
        buf[3] = 0xF0;
        buf[2] = ((gStripStepWidth * 5) << 6) >> 1;
        gClearImage(buf, 0, 0, 0);
        func_80092C8C(0);
    }

    if ((arg1 & 0xFF) != 0) {
        gVSyncWait(0);
        func_800932F0((u8 *)gMovieSliceContext + 0x34 + (1 - gMovieSliceContext->unk114) * 92);
        gPutDispEnvCached((u8 *)gMovieSliceContext + 0xEC + (1 - gMovieSliceContext->unk114) * 20);
        func_80092BF4(1);
    }

    func_80095E90(0);
}
