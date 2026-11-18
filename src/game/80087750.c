// Categories: Platform > Sound > Shutdown | Platform > SPU
#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

typedef struct {
    u8 pad[0x7B10];
    void *unk7B10;
    SpuIrqCallback unk7B14;
    s32 unk7B18;
    volatile s32 unk7B1C;
    volatile s32 unk7B20;
    volatile s32 unk7B24;
    u8 pad2[0x20];
    volatile s32 unk7B48;
} Unk8009F4D8;

extern Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");

extern void func_8008C9C0(void);
extern void gHaltMovieAudioStream(void) __asm__("func_800876EC");
extern void gSpuSetIrqEnable(s32 arg) __asm__("func_80098A78");
extern SpuIrqCallback gSpuInstallIrqCallback(SpuIrqCallback callback) __asm__("func_80098BF8");
extern void func_8008C9E0(void);
extern void gSpuSetTransferCallback(s32 value) __asm__("func_80098FB8");
extern void func_80091510(void *argument);
extern void gResetMovieAudioState(void) __asm__("func_80087688");

/**
 * @brief Shuts the sound subsystem down: stops the DMA, waits for the
 *        pending flag and restores the saved callbacks.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Under func_8008C9C0: gHaltMovieAudioStream, gSpuSetIrqEnable(0), then
 * gSpuInstallIrqCallback(gMovieAudioState->unk7B14) restores the IRQ callback;
 * func_8008C9E0 unmask. A busy loop waits for unk7B48 == 0. Under mask
 * again: gSpuSetTransferCallback(unk7B18) restores the SPU DMA latch, unk7B20 = 0,
 * unk7B1C != 0 installs func_80091510(unk7B10), unk7B24 = 0; unmask and
 * gResetMovieAudioState finish.
 *
 * @par Likely role
 * The teardown counterpart of gInitMovieAudio (both called by
 * gPlayVideoFile): the busy wait drains the in-flight transfer before the
 * latch is restored, so no IRQ fires into a freed state.
 *
 * @par Unknown
 * What unk7B48 counts (likely the pending DMA words).
 *
 * @par Callers
 * gPlayVideoFile (verified in the retail listing; the call-graph edge is
 * missing).
 */
void gShutdownMovieAudio(void) __asm__("func_80087750");

void gShutdownMovieAudio(void)
{
    func_8008C9C0();
    gHaltMovieAudioStream();
    gSpuSetIrqEnable(0);

    gSpuInstallIrqCallback(gMovieAudioState->unk7B14);
    func_8008C9E0();

    while (gMovieAudioState->unk7B48 != 0) {
    }

    func_8008C9C0();
    gSpuSetTransferCallback(gMovieAudioState->unk7B18);
    gMovieAudioState->unk7B20 = 0;
    if (gMovieAudioState->unk7B1C != 0) {
        func_80091510(gMovieAudioState->unk7B10);
    }
    gMovieAudioState->unk7B24 = 0;
    func_8008C9E0();
    gResetMovieAudioState();
}
