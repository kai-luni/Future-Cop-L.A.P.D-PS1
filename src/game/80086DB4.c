// Categories: Platform > Audio
#include "game/types.h"

typedef struct {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x7B30];
    s32 unk7B3C;
    u8 pad7B40[8];
    s32 unk7B48;
    s32 unk7B4C;
    u8 *unk7B50;
    s32 unk7B54;
    u8 pad7B58[0xC];
    u32 unk7B64;
    u32 unk7B68;
    u32 unk7B6C;
    u32 unk7B70;
} Unk80086DB4;

extern volatile Unk80086DB4 *gMovieAudioState __asm__("D_8009F4D8");

extern void *func_800869D8(u32 index);
extern void *func_80086A4C(u32 arg);
extern void func_8008684C(u8 *arg0, u32 arg1, u32 arg2);
extern void gServiceMovieAudioIrq(void) __asm__("func_80086AC0");
extern u32 func_80098788(s32 on_off, u32 voice_bits);
extern void func_80098C68(s32 a, u32 b);

/**
 *  @brief Initializes audio transfer state once and updates two voice masks.
 *
 *  On first use, prepares three 0x300-byte transfers and waits for each to
 *  finish before performing the final initialization step.
 *  @par Globals
 *  gMovieAudioState holds transfer flags, buffers, initialization state, and voice masks.
 *  @par Control flow
 *  The initialization branch loops three times and busy-waits on unk7B48;
 *  voice-mask operations run on every call.
 *  @par Calls
 *  func_800869D8 and func_80086A4C obtain buffers; func_8008684C starts each
 *  transfer; func_80086AC0 performs the final initialization step.
 *  func_80098788 processes each mask; func_80098C68 receives their union.
 *  @par Side effects
 *  Updates shared transfer and mask state and invokes audio operations.
 *  @par Callers
 *  Confirmed: gMovieStreamDemux.
 */
void gMovieAudioStart(void) __asm__("func_80086DB4");

void gMovieAudioStart(void)
{
    u8 *buf;
    u8 *other;
    u32 i;

    if (gMovieAudioState->unk08 == 0) {
        gMovieAudioState->unk7B3C = 0x300;
        for (i = 0; i < 3; i++) {
            buf = func_800869D8(0x300);
            if (buf != 0) {
                other = func_80086A4C(0x300);
            } else {
                buf = (u8 *)gMovieAudioState + 0x7810;
                other = buf;
            }
            gMovieAudioState->unk7B48 = 1;
            gMovieAudioState->unk7B4C = 1;
            gMovieAudioState->unk7B50 = other;
            gMovieAudioState->unk7B54 = 1;
            func_8008684C(buf, gMovieAudioState->unk7B3C, 0);
            gMovieAudioState->unk7B3C = gMovieAudioState->unk7B3C + 0x300;
            while (gMovieAudioState->unk7B48 != 0) {
            }
        }
        gMovieAudioState->unk7B48 = 3;
        gMovieAudioState->unk08 = 1;
        gServiceMovieAudioIrq();
        gMovieAudioState->unk7B48 = 0;
    }
    gMovieAudioState->unk7B6C = func_80098788(0, gMovieAudioState->unk7B64);
    gMovieAudioState->unk7B70 = func_80098788(0, gMovieAudioState->unk7B68);
    func_80098C68(1, gMovieAudioState->unk7B64 | gMovieAudioState->unk7B68);
}
