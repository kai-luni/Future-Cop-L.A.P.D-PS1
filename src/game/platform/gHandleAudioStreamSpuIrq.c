// Categories: Platform > Audio Streaming
#include "game/types.h"

struct Stream22FA8 {
    u8 pad00[0x04];
    s32 active;      /* 0x04 */
    u8 pad08[0x14];
    u32 rate;        /* 0x1C */
    u8 pad20[0x14];
    s32 start;       /* 0x34 */
    u32 length;      /* 0x38 */
    u8 looping;      /* 0x3C */
    u8 pad3D[3];
    u32 unk40;       /* 0x40 */
    s32 unk44;       /* 0x44 */
    u8 pad48[0x24];
    u32 unk6C;       /* 0x6C */
    u32 unk70;       /* 0x70 */
    u32 unk74;       /* 0x74 */
    u8 pad78[4];
    u32 unk7C;       /* 0x7C */
    s32 ready;       /* 0x80 */
    s32 unk84;       /* 0x84 */
    s32 unk88;       /* 0x88 */
    s32 unk8C;       /* 0x8C */
    s32 busy;        /* 0x90 */
};

extern struct Stream22FA8 gMixerState __asm__("D_800A0968");
extern s32 gStreamDmaChannelIndex[2] __asm__("D_800A0988");
extern s32 gStreamChannelStart[2] __asm__("D_800A099C");
extern s32 gStreamIrqBusy[2] __asm__("D_800A09F8");
extern s32 gFrameTickCounter[2] __asm__("D_8009FA2C");
extern u32 gStreamEventCallback __asm__("D_8009F634");

extern void gDisarmAudioStreamSpuIrq(void) __asm__("func_80022F7C");
extern void gSubmitAudioStreamTransfer(void) __asm__("func_80022D84");
extern void gSetSpuIrqAddress(s32 address) __asm__("func_80098BB8");
extern void gSpuSetIrqEnable(s32 mode) __asm__("func_80098A78");

void gHandleAudioStreamSpuIrq(void) __asm__("func_80022FA8");

/**
 * @brief Advances streamed audio playback and arms the next SPU interrupt.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Disarms the IRQ, rejects reentry through mixer +0x90, then handles inactive and flagged
 * states. It replenishes the block counter, may emit callback codes 3, 4 or 6, and advances or
 * wraps the offset before programming the next IRQ and clearing the busy word.
 *
 * @par Globals
 * Reads and writes gMixerState, including gStreamDmaChannelIndex, gStreamChannelStart and
 * gStreamIrqBusy. Reads gFrameTickCounter and gStreamEventCallback.
 *
 * @par Callers
 * No direct JAL callers in the retail listing.
 *
 * @par Role and path
 * Registered by gStartAudioStreamChannels as the SPU IRQ callback using the retail alias
 * gHandleAudioStreamSpuIrq.
 *
 * @par Unknown
 * The event codes' meanings for callers outside this stream setup are unresolved.
 */
void gHandleAudioStreamSpuIrq(void)
{
    register u32 quot __asm__("$3");
    gDisarmAudioStreamSpuIrq();
    if (gMixerState.busy != 0) {
        return;
    }
    gMixerState.busy = 1;
    if (gMixerState.active == 0) {
        gStreamIrqBusy[0] = 0;
        return;
    }
    if (gMixerState.unk84 != 0) {
        if (gStreamEventCallback != 0) {
            ((void (*)(s32))gStreamEventCallback)(6);
        }
        gStreamIrqBusy[0] = 0;
        return;
    }
    if (gMixerState.unk74 < gMixerState.unk70) {
        gMixerState.unk74++;
    }
    if (gMixerState.ready != 0) {
        if (gMixerState.start != 0) {
            gStreamDmaChannelIndex[0] = 0;
            gSubmitAudioStreamTransfer();
        } else {
            if (gStreamEventCallback != 0) {
                ((void (*)(s32))gStreamEventCallback)(3);
            }
            if (gMixerState.start != 0) {
                gStreamDmaChannelIndex[0] = 0;
                gSubmitAudioStreamTransfer();
            } else {
                if (gMixerState.unk74 == gMixerState.unk70 - 1) {
                    if (gStreamEventCallback != 0) {
                        ((void (*)(s32))gStreamEventCallback)(4);
                    }
                }
                if (gStreamChannelStart[0] != 0) {
                    gStreamDmaChannelIndex[0] = 0;
                    gSubmitAudioStreamTransfer();
                }
            }
        }
    }
    if (gMixerState.looping != 0 && gMixerState.unk40 == gMixerState.unk7C) {
        quot = (gMixerState.length * 3 + (gMixerState.length >> 1)) * 300 / gMixerState.rate;
        gMixerState.unk84 = 1;
        {
            u32 tmp = gMixerState.unk40 - 16;
            gMixerState.unk7C = tmp + gMixerState.length;
        }
        gMixerState.unk88 = gFrameTickCounter[0] + (quot + 10) * 3;
    } else {
        u32 product;
        product = gMixerState.unk70 * gMixerState.unk6C;
        gMixerState.unk7C += gMixerState.unk6C;
        if (gMixerState.unk7C >= product) {
            gMixerState.unk7C = 0;
        }
        __asm__ volatile("" ::: "memory");
        gMixerState.unk88 = gFrameTickCounter[0] + gMixerState.unk8C;
    }
    if (gMixerState.unk88 < 20) {
        gMixerState.unk88 = 20;
    }
    gSetSpuIrqAddress(gMixerState.unk44 + gMixerState.unk7C);
    gSpuSetIrqEnable(1);
    gStreamIrqBusy[0] = 0;
}
