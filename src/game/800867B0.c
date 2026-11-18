// Categories: Game Engine > Value Scaling
#include "game/types.h"

typedef struct {
    u8 pad00[0x7B74];
    s32 unk7B74;
    s32 unk7B78;
    s32 unk7B7C;
} Unk8009F4D8;

extern Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");
extern s32 func_800966B0(unsigned long long p, s32 a, s32 b);

/**
 *  @brief Scales an input according to the current state's mode and value.
 *
 *  Selects one of three multiplier and divisor pairs from the state, or
 *  returns zero for modes other than 0 and 1.
 *  @param arg0 Unsigned input value to scale.
 *  @return The helper result for a selected pair, or zero for other modes.
 *  @par Globals
 *  gMovieAudioState supplies the mode and the value tested against 0x800.
 *  @par Control flow
 *  Mode 1 selects 0x40 and 0x3F; mode 0 selects a pair based on the value.
 *  @par Calls
 *  func_800966B0 receives the 64-bit product and selected divisor words.
 *  @par Side effects
 *  No direct state changes; effects of the helper are not shown here.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_800867B0(u32 arg0)
{
    register s32 ret __asm__("$3") = 0;
    register s32 mode __asm__("$5");
    register u32 k __asm__("$2");
    register s32 a2 __asm__("$6");
    register s32 a3 __asm__("$7");
    register s32 val __asm__("$3");
    unsigned long long p;

    mode = gMovieAudioState->unk7B74;
    if (mode != 0) {
        if (mode == 1) {
            __asm__ volatile("");
            k = 0x40;
            __asm__ volatile("addiu\t%0,$0,0" : "=r"(a3));
            a2 = 0x3F;
            __asm__ volatile("" : "+r"(a2));
            ret = func_800966B0((unsigned long long)arg0 * k, a2, a3);
        }
    } else {
        val = gMovieAudioState->unk7B7C;
        if (val == 0x800) {
            __asm__ volatile("");
            k = 0x14D0;
            __asm__ volatile("addiu\t%0,$0,0" : "=r"(a3));
            a2 = 0x1117;
            __asm__ volatile("" : "+r"(a2));
            ret = func_800966B0((unsigned long long)arg0 * k, a2, a3);
        } else {
            k = 0x6D440;
            p = (unsigned long long)arg0 * k;
            __asm__ volatile("addiu\t%0,$0,0" : "=r"(a3));
            a2 = 0x82BA3;
            ret = func_800966B0(p, a2, a3);
        }
    }

    return ret;
}
