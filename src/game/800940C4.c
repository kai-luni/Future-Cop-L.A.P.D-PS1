// Categories: Platform > GPU | Graphics > Textures
#include "game/types.h"
s32 gBuildTexWindowCommand(u8 *p) __asm__("func_800940C4");


/**
 * @brief Builds a GPU E2h texture-window command from an 8-byte rectangle.
 *
 * @param p Rectangle: bytes 0/2 the unsigned origin, s16 at +4/+6 the signed
 *        extents (negated before packing).
 * @return The packed E2h command word, or 0 for a null pointer.
 *
 * @par Confirmed
 * v0 = p[0] >> 3, v1 = p[2] >> 3, v2 = (-s16[p+4] & 0xFF) >> 3,
 * v3 = (-s16[p+6] & 0xFF) >> 3; result = 0xE2000000 | (v1 << 15) |
 * (v0 << 10) | (v3 << 5) | v2. E2h is the PS1 texture-window register:
 * the four 5-bit fields are the window mask/offset pairs in 8-pixel units.
 *
 * @par Likely role
 * The texture-window helper of the draw-mode builders func_80093954,
 * func_80093A9C, func_80093AF0 and func_80093CE8: it clips atlas sprites
 * to their cell before the GPU samples them.
 *
 * @par Callers
 * func_80093954, func_80093A9C, func_80093AF0, func_80093CE8.
 *
 * @par Environment path
 * Encodes an 8-byte texture window rectangle into a GP0 0xE2 texture-window
 * command word and returns it (called by func_80093954, func_80093A9C,
 * func_80093AF0).
 */
s32 gBuildTexWindowCommand(u8 *p)
{
    s32 v[4];
    s32 r;

    if (p == 0) {
        r = 0;
    } else {
        v[0] = p[0] >> 3;
        v[2] = (-*(s16 *)(p + 4) & 0xFF) >> 3;
        v[1] = p[2] >> 3;
        v[3] = (-*(s16 *)(p + 6) & 0xFF) >> 3;
        r = 0xE2000000 | (v[1] << 15) | (v[0] << 10) | (v[3] << 5) | v[2];
    }
    return r;
}
