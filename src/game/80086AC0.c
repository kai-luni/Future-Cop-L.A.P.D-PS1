// Categories: Game Engine > Streaming
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x7B18];
    s32 unk7B24;
    u8 pad7B28[4];
    s32 unk7B2C[4];
    s32 unk7B3C;
    s32 unk7B40;
    s32 unk7B44;
    s32 unk7B48;
    s32 unk7B4C;
    void *unk7B50;
    s32 unk7B54;
    s32 unk7B58;
    u32 unk7B5C;
} Unk8009F4D8;

extern volatile Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");

extern void gSpuSetIrqEnable(s32) __asm__("func_80098A78");
extern void gSpuInstallIrqCallback(void *) __asm__("func_80098BF8");
extern void *func_800869D8(u32);
extern void *func_80086A4C(u32);
extern void func_8008684C(void *, s32, s32);
extern s32 func_800867B0(s32);
extern void func_80098BB8(u32);
void func_80086AC0(void);

/**
 *  @brief Services an active buffered stream and schedules its next callback.
 *
 *  The routine manages a pending 0x300-byte buffer request and advances a cyclic output position.
 *  If the primary buffer request fails, it submits an embedded fallback buffer instead.
 *  @par Globals
 *  gMovieAudioState points to the volatile stream state, buffer fields, counters and cyclic address base.
 *  @par Control flow
 *  An inactive stream returns immediately; otherwise buffer submission depends on pending state and a counter threshold.
 *  On even counter values, a nonzero entry in a four-element table is processed before the counter advances.
 *  @par Calls
 *  gSpuSetIrqEnable and gSpuInstallIrqCallback control callback setup; func_800869D8 and func_80086A4C obtain buffers.
 *  func_8008684C submits a buffer; func_800867B0 processes a table value; func_80098BB8 receives the cyclic address.
 *  @par Side effects
 *  Updates stream state, submits data, changes callback configuration and registers func_80086AC0 for the next callback.
 *  @par Callers
 *  Unknown: no direct callers are listed; the function registers itself as a callback.
 */
void func_80086AC0(void)
{
    void *buf;

    if (gMovieAudioState->unk08 == 0) {
        return;
    }

    gSpuSetIrqEnable(0);
    gSpuInstallIrqCallback(0);

    if (gMovieAudioState->unk7B48 == 0) {
        if ((gMovieAudioState->unk7B44 * 2 - gMovieAudioState->unk7B40) < 6) {
            buf = func_800869D8(0x300);
            if (buf != 0) {
                void *other = func_80086A4C(0x300);
                gMovieAudioState->unk7B58 = 1;
                gMovieAudioState->unk7B4C = 1;
                gMovieAudioState->unk7B48 = 1;
                gMovieAudioState->unk7B50 = other;
                gMovieAudioState->unk7B54 = gMovieAudioState->unk7B3C;
                func_8008684C(buf, gMovieAudioState->unk7B3C, 0);
                gMovieAudioState->unk7B3C += 0x300;
            } else {
                gMovieAudioState->unk7B58 = 0;
                gMovieAudioState->unk7B4C = 0;
                gMovieAudioState->unk7B48 = 1;
                gMovieAudioState->unk7B50 = (u8 *)gMovieAudioState + 0x7810;
                gMovieAudioState->unk7B54 = 0;
                func_8008684C((u8 *)gMovieAudioState + 0x7810, 0, 0);
            }
        }
    }

    if ((gMovieAudioState->unk7B40 & 1) == 0) {
        s32 val = gMovieAudioState->unk7B2C[(gMovieAudioState->unk7B40 >> 1) & 3];
        if (val != 0) {
            gMovieAudioState->unk7B24 = func_800867B0(val);
        }
    }

    gMovieAudioState->unk7B40++;
    func_80098BB8(gMovieAudioState->unk7B5C + (gMovieAudioState->unk7B40 & 7) * 0x180);
    gSpuInstallIrqCallback(func_80086AC0);
    gSpuSetIrqEnable(1);
}
