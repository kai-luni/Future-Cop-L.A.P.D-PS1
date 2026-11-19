// Categories: Resources > Video Decode | Platform > GPU | Platform > Interrupts
#include "game/types.h"

typedef void (*DecDctCallback)(void);

typedef struct {
    u8 pad[0x120];
    DecDctCallback unk120;
    DecDctCallback unk124;
    DecDctCallback unk128;
} Unk8009F4BC;

extern Unk8009F4BC *gMovieSliceContext __asm__("D_8009F4BC");

extern s32 gMdecInSync(s32 mode) __asm__("func_80096090");
extern s32 gMdecOutSync(s32 mode) __asm__("func_800960CC");
extern void func_80092C8C(s32 arg);
extern void func_8008C9C0(void);
extern DecDctCallback func_8009612C(DecDctCallback callback);
extern DecDctCallback gMdecSetInCallback(DecDctCallback callback) __asm__("func_80096108");
extern void func_80092B94(DecDctCallback callback);
extern void func_8008C9E0(void);
extern void func_80095E90(s32 option);

/**
 * @brief Stops the running MDEC movie and restores the saved DMA callbacks,
 *        all inside a COP0 critical section.
 *
 * @return Nothing; a no-op when no decode context is active (gMovieSliceContext == 0).
 *
 * @par Confirmed
 * With an active context: func_80096090(0) and func_800960CC(0) stop the two
 * decode DMA channels, func_80092C8C(0) waits for the GPU, func_8008C9C0
 * disables interrupts (COP0 mask 0x402). The three saved callbacks at
 * context +0x120/+0x124/+0x128 are re-installed through func_8009612C,
 * func_80096108 and func_80092B94, then cleared. func_8008C9E0 re-enables
 * interrupts, func_80095E90(1) restores the display interrupt state, and
 * the context pointer is nulled.
 *
 * @par Likely role
 * The movie teardown of the video layer gCloseVideoLayer: the decode was
 * hijacking the DMA callbacks, so stopping it must hand them back to the
 * previous owners (sound stream, display) atomically.
 *
 * @par Callers
 * gCloseVideoLayer.
 */
void gMovieStopAndRestoreCallbacks(void) __asm__("func_80085A24");

void gMovieStopAndRestoreCallbacks(void)
{
    if (gMovieSliceContext != 0) {
        gMdecInSync(0);
        gMdecOutSync(0);
        func_80092C8C(0);
        func_8008C9C0();

        func_8009612C(gMovieSliceContext->unk120);
        gMdecSetInCallback(gMovieSliceContext->unk124);
        func_80092B94(gMovieSliceContext->unk128);

        gMovieSliceContext->unk120 = 0;
        gMovieSliceContext->unk124 = 0;
        gMovieSliceContext->unk128 = 0;

        func_8008C9E0();
        func_80095E90(1);
        gMovieSliceContext = 0;
    }
}
