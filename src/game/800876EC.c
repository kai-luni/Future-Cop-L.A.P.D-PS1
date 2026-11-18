// Categories: Platform > Sound | Platform > SPU
#include "game/types.h"

typedef struct {
    u8 pad0[0x8];
    volatile s32 unk8;
    u8 padC[0x7B58];
    volatile u32 unk7B64;
    volatile u32 unk7B68;
    volatile s32 unk7B6C;
    volatile s32 unk7B70;
} Unk8009F4D8;

extern Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");
extern void func_80098C68(s32, u32);
extern u32 func_80098788(s32, u32);

/**
 * @brief Halts stream playback in gMovieAudioState, keys off its voices, and resets SPU registers 0xCC/0xCD.
 *
 * Confirmed: clears gMovieAudioState->unk8 (+0x08), calls func_80098C68(0, unk7B64 | unk7B68) to key off both voice bitmasks, and calls func_80098788 with modes unk7B6C (+0x7B6C) and unk7B70 (+0x7B70) for voice masks unk7B64 (+0x7B64) and unk7B68 (+0x7B68).
 *
 * @param void No parameters.
 *
 * @par Globals
 * gMovieAudioState - 0x7B80-byte audio stream state (+0x08 cleared; +0x7B64/+0x7B68 voice masks and +0x7B6C/+0x7B70 modes read).
 *
 * @par Control flow
 * Straight-line clear of gMovieAudioState->unk8 followed by three unconditional SPU calls; no branches or loops.
 *
 * @par Calls
 * func_80098C68 - keys off voice mask (unk7B64 | unk7B68) with mode 0; func_80098788 - drives SPU registers 0xCC/0xCD for unk7B64 and unk7B68.
 *
 * @par Side effects
 * Clears gMovieAudioState->unk8 (+0x08) and updates SPU voice key-off and register state via callees.
 *
 * @par Callers
 * Confirmed: gShutdownMovieAudio (0x80087760 in asm/us/main.s, audio stream shutdown) / Likely: none / Unknown: none.
 */
void gHaltMovieAudioStream(void) __asm__("func_800876EC");

void gHaltMovieAudioStream(void)
{
    gMovieAudioState->unk8 = 0;
    func_80098C68(0, gMovieAudioState->unk7B64 | gMovieAudioState->unk7B68);
    func_80098788(gMovieAudioState->unk7B6C, gMovieAudioState->unk7B64);
    func_80098788(gMovieAudioState->unk7B70, gMovieAudioState->unk7B68);
}
