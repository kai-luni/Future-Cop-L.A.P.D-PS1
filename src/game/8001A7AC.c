// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

extern int gSolveLobVector();

extern s32 func_8001A030(s32 arg0);
extern s32 func_80095900(s32 arg0);
extern void func_8001ABE0(u32 arg0, s32 arg1);

/**
 * @brief Scalar normalisation helper: computes the reciprocal-root scale of
 *        a squared magnitude and applies it in place.
 *
 * @param arg0 The value (passed through func_8001A030, likely a square or
 *        magnitude preparation).
 * @return s0 = func_80095900(func_8001A030(arg0)) << 2, the length word.
 *
 * @par Confirmed
 * When s0 >= 2, func_8001ABE0(0x80000000 / (s0 >> 1), arg0) applies the
 * reciprocal scale; smaller values skip the division (guard against a
 * zero/near-zero length).
 *
 * @par Likely role
 * The scalar sibling of the vector normaliser func_8001A180, used by the
 * mover physics and the lob solver gSolveLobVector to normalise single
 * components or magnitudes with the same reciprocal table.
 *
 * @par Unknown
 * What func_8001A030 prepares and func_8001ABE0 writes back.
 *
 * @par Callers
 * func_80051EB8, func_80053A74, func_80054590, func_800549F0, gSolveLobVector.
 */
s32 func_8001A7AC(s32 arg0)
{
    s32 s0;

    s0 = func_80095900(func_8001A030(arg0)) << 2;
    if (s0 >= 2) {
        func_8001ABE0(0x80000000u / (u32)(s0 >> 1), arg0);
    }
    return s0;
}
