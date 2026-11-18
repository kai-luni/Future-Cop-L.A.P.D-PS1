// Categories: Resources > Movie Playback | Graphics > Video Decode
#include "game/types.h"
typedef struct { u8 unk00[0x7B74]; s32 unk7B74; } U;
extern U *gMovieAudioState __asm__("D_8009F4D8");
extern s32 func_800966B0(u32 lo, u32 hi, s32 a, s32 b);

/**
 * @brief Converts a movie video chunk index into a playback pacing threshold in stream units.
 *
 * When gMovieAudioState->unk7B74 != 1, returns arg0 << 10; when it equals 1, computes the 64-bit
 * unsigned product arg0 * 0x271000 via multu and divides it by 0xBB5 (2997) via func_800966B0.
 *
 * @param arg0 Video frame/chunk counter index.
 * @return Pacing threshold compared against gMovieAudioClockRead() in gMovieStreamDemux.
 *
 * @par Globals
 * gMovieAudioState - video/audio stream decoder state (reads mode word at +0x7B74).
 *
 * @par Control flow
 * Single branch on gMovieAudioState->unk7B74 != 1 returning arg0 << 10 vs 64-bit division via func_800966B0.
 *
 * @par Calls
 * func_800966B0 - performs 64-bit division of (arg0 * 0x271000) by 0xBB5 (2997).
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: gMovieStreamDemux (0x80087CA4, computes frame presentation deadline before polling gMovieAudioClockRead) / Likely: none / Unknown: none.
 */
s32 gMovieFrameDeadline(u32 arg0) __asm__("func_80086F58");

s32 gMovieFrameDeadline(u32 arg0)
{
    if (gMovieAudioState->unk7B74 != 1) {
        return arg0 << 10;
    }
    {
        register u32 k __asm__("$2") = 0x271000;
        register u32 lo __asm__("$4");
        register u32 hi __asm__("$5");
        register s32 a2 __asm__("$6");
        register s32 a3 __asm__("$7");
        __asm__ volatile("multu\t%0,%1" : : "r"(arg0), "r"(k));
        __asm__ volatile("addiu\t%0,$0,0" : "=r"(a3));
        a2 = 0xBB5;
        __asm__ volatile("mfhi\t%0" : "=r"(hi) : "r"(a2), "r"(a3));
        __asm__ volatile("mflo\t%0" : "=r"(lo) : "r"(hi));
        return func_800966B0(lo, hi, a2, a3);
    }
}
