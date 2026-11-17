// Categories: Resources > Video Decode | Platform > GPU | Platform > Interrupts
#include "game/types.h"

typedef void (*DecDctCallback)(void);

typedef struct {
    u8 pad[0x120];
    DecDctCallback unk120;
    DecDctCallback unk124;
    DecDctCallback unk128;
} Unk8009F4BC;

extern Unk8009F4BC *gDecodeJob __asm__("D_8009F4BC");

extern s32 func_80096090(s32 mode);
extern s32 func_800960CC(s32 mode);
extern void func_80092C8C(s32 arg);
extern void func_8008C9C0(void);
extern DecDctCallback func_8009612C(DecDctCallback callback);
extern DecDctCallback func_80096108(DecDctCallback callback);
extern void func_80092B94(DecDctCallback callback);
extern void func_8008C9E0(void);
extern void func_80095E90(s32 option);

/**
 * @brief Stops the running MDEC movie and restores the saved DMA callbacks,
 *        all inside a COP0 critical section.
 *
 * @return Nothing; a no-op when no decode context is active (D_8009F4BC == 0).
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
 * The movie teardown of the video layer func_80087D48: the decode was
 * hijacking the DMA callbacks, so stopping it must hand them back to the
 * previous owners (sound stream, display) atomically.
 *
 * @par Callers
 * func_80087D48.
 */
void func_80085A24(void)
{
    if (gDecodeJob != 0) {
        func_80096090(0);
        func_800960CC(0);
        func_80092C8C(0);
        func_8008C9C0();

        func_8009612C(gDecodeJob->unk120);
        func_80096108(gDecodeJob->unk124);
        func_80092B94(gDecodeJob->unk128);

        gDecodeJob->unk120 = 0;
        gDecodeJob->unk124 = 0;
        gDecodeJob->unk128 = 0;

        func_8008C9E0();
        func_80095E90(1);
        gDecodeJob = 0;
    }
}
