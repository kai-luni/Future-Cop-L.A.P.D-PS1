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

extern struct Stream22FA8 D_800A0968;
extern s32 D_800A0988[2];
extern s32 D_800A099C[2];
extern s32 D_800A09F8[2];
extern s32 D_8009FA2C[2];
extern u32 D_8009F634;

extern void func_80022F7C(void);
extern void func_80022D84(void);
extern void func_80098BB8(s32 address);
extern void func_80098A78(s32 mode);

/**
 *  @brief Advances stream state and schedules the next audio transfer.
 *
 *  Checks stream readiness, invokes optional status callbacks, and computes the
 *  next buffer offset and deadline, including a special looping transition.
 *  @par Globals
 *  D_800A0968 holds stream state; D_800A0988, D_800A099C, and D_800A09F8 hold control state.
 *  D_8009FA2C supplies the time base; D_8009F634 holds an optional callback address.
 *  @par Control flow
 *  Returns early for busy, inactive, or flagged streams; ready streams may dispatch callbacks.
 *  The callback can change start state before it is checked again.
 *  Looping and normal paths calculate offsets and deadlines differently; the deadline is clamped.
 *  @par Calls
 *  func_80022F7C runs first; func_80022D84 handles a ready stream when requested.
 *  func_80098BB8 sets the transfer address and func_80098A78 starts mode 1.
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
    if (D_800A0968.busy != 0) {
        return;
    }
    D_800A0968.busy = 1;
    if (D_800A0968.active == 0) {
        D_800A09F8[0] = 0;
        return;
    }
    if (D_800A0968.unk84 != 0) {
        if (D_8009F634 != 0) {
            ((void (*)(s32))D_8009F634)(6);
        }
        D_800A09F8[0] = 0;
        return;
    }
    if (D_800A0968.unk74 < D_800A0968.unk70) {
        D_800A0968.unk74++;
    }
    if (D_800A0968.ready != 0) {
        if (D_800A0968.start != 0) {
            D_800A0988[0] = 0;
            func_80022D84();
        } else {
            if (D_8009F634 != 0) {
                ((void (*)(s32))D_8009F634)(3);
            }
            if (D_800A0968.start != 0) {
                D_800A0988[0] = 0;
                func_80022D84();
            } else {
                if (D_800A0968.unk74 == D_800A0968.unk70 - 1) {
                    if (D_8009F634 != 0) {
                        ((void (*)(s32))D_8009F634)(4);
                    }
                }
                if (D_800A099C[0] != 0) {
                    D_800A0988[0] = 0;
                    func_80022D84();
                }
            }
        }
    }
    if (D_800A0968.looping != 0 && D_800A0968.unk40 == D_800A0968.unk7C) {
        quot = (D_800A0968.length * 3 + (D_800A0968.length >> 1)) * 300 / D_800A0968.rate;
        D_800A0968.unk84 = 1;
        {
            u32 tmp = D_800A0968.unk40 - 16;
            D_800A0968.unk7C = tmp + D_800A0968.length;
        }
        D_800A0968.unk88 = D_8009FA2C[0] + (quot + 10) * 3;
    } else {
        u32 product;
        product = D_800A0968.unk70 * D_800A0968.unk6C;
        D_800A0968.unk7C += D_800A0968.unk6C;
        if (D_800A0968.unk7C >= product) {
            D_800A0968.unk7C = 0;
        }
        __asm__ volatile("" ::: "memory");
        D_800A0968.unk88 = D_8009FA2C[0] + D_800A0968.unk8C;
    }
    if (D_800A0968.unk88 < 20) {
        D_800A0968.unk88 = 20;
    }
    func_80098BB8(D_800A0968.unk44 + D_800A0968.unk7C);
    func_80098A78(1);
    D_800A09F8[0] = 0;
}
