// Categories: Platform > Sound > Shutdown
#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    u8 pad[0x7B14];
    volatile s32 unk7B24;
    s32 unk7B28;
    s32 unk7B2C[4];
    s32 unk7B3C;
} Unk8009F4D8;

extern Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");
extern void gDisableCpuInterrupts(void) __asm__("func_8008C9C0");
extern void gEnableCpuInterrupts(void) __asm__("func_8008C9E0");

/**
 * @brief Final sound-state reset: clears the head words and the four
 *        channel words, marks the state idle.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Under func_8008C9C0: gMovieAudioState->unk0/unk4/unkC = 0, the four words at
 * +0x7B2C = 0, +0x7B3C = 1, the volatile +0x7B24 = 0; func_8008C9E0
 * unmask.
 *
 * @par Likely role
 * The last step of the sound shutdown gShutdownMovieAudio: after the DMA drain
 * this zeroes the transfer state so a later re-init starts clean, the 1
 * at +0x7B3C likely marks "initialised/idle".
 *
 * @par Callers
 * gShutdownMovieAudio.
 */
void gResetMovieAudioState(void) __asm__("func_80087688");

void gResetMovieAudioState(void)
{
    u32 i;

    gDisableCpuInterrupts();

    gMovieAudioState->unk0 = 0;
    gMovieAudioState->unk4 = 0;
    gMovieAudioState->unkC = 0;

    for (i = 0; i < 4; i++) {
        gMovieAudioState->unk7B2C[i] = 0;
    }

    gMovieAudioState->unk7B3C = 1;
    gMovieAudioState->unk7B24 = 0;

    gEnableCpuInterrupts();
}
