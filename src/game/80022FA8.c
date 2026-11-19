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
extern s32 D_800A0988[2];
extern s32 D_800A099C[2];
extern s32 D_800A09F8[2];
extern s32 gFrameTickCounter[2] __asm__("D_8009FA2C");
extern u32 gStreamEventCallback __asm__("D_8009F634");

extern void func_80022F7C(void);
extern void func_80022D84(void);
extern void func_80098BB8(s32 address);
extern void gSpuSetIrqEnable(s32 mode) __asm__("func_80098A78");

/**
 *  @brief Advances stream state and schedules the next audio transfer.
 *
 *  Checks stream readiness, invokes optional status callbacks, and computes the
 *  next buffer offset and deadline, including a special looping transition.
 *  @par Globals
 *  D_800A0968 holds stream state; D_800A0988, D_800A099C, and D_800A09F8 hold control state.
 *  D_8009FA2C supplies the time base; gStreamEventCallback holds an optional callback address.
 *  @par Control flow
 *  Returns early for busy, inactive, or flagged streams; ready streams may dispatch callbacks.
 *  The callback can change start state before it is checked again.
 *  Looping and normal paths calculate offsets and deadlines differently; the deadline is clamped.
 *  @par Calls
 *  func_80022F7C runs first; func_80022D84 handles a ready stream when requested.
 *  func_80098BB8 sets the transfer address and gSpuSetIrqEnable starts mode 1.
 *  @par Side effects
 *  Updates stream fields and control state, invokes callbacks, and schedules a transfer.
 *  The busy field is set before inactive and flagged-stream early returns.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80022FA8(void)
{
    register u32 quot __asm__("$3");
    func_80022F7C();
    if (gMixerState.busy != 0) {
        return;
    }
    gMixerState.busy = 1;
    if (gMixerState.active == 0) {
        D_800A09F8[0] = 0;
        return;
    }
    if (gMixerState.unk84 != 0) {
        if (gStreamEventCallback != 0) {
            ((void (*)(s32))gStreamEventCallback)(6);
        }
        D_800A09F8[0] = 0;
        return;
    }
    if (gMixerState.unk74 < gMixerState.unk70) {
        gMixerState.unk74++;
    }
    if (gMixerState.ready != 0) {
        if (gMixerState.start != 0) {
            D_800A0988[0] = 0;
            func_80022D84();
        } else {
            if (gStreamEventCallback != 0) {
                ((void (*)(s32))gStreamEventCallback)(3);
            }
            if (gMixerState.start != 0) {
                D_800A0988[0] = 0;
                func_80022D84();
            } else {
                if (gMixerState.unk74 == gMixerState.unk70 - 1) {
                    if (gStreamEventCallback != 0) {
                        ((void (*)(s32))gStreamEventCallback)(4);
                    }
                }
                if (D_800A099C[0] != 0) {
                    D_800A0988[0] = 0;
                    func_80022D84();
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
    func_80098BB8(gMixerState.unk44 + gMixerState.unk7C);
    gSpuSetIrqEnable(1);
    D_800A09F8[0] = 0;
}
