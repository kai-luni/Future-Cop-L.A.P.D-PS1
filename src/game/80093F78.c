// Categories: Graphics > GPU Primitives > Draw Area
#include "game/types.h"

extern u16 D_8009DDF4;
extern u16 D_8009DDF6;

/**
 * @brief Builds the GPU draw-area top-left word (opcode 0xE3) with screen
 *        clamping.
 *
 * @param x, y Corner coordinates.
 * @return 0xE3000000 | (y & 0x3FF) << 10 | (x & 0x3FF), coordinates clamped
 *         to [0, D_8009DDF4-1] / [0, D_8009DDF6-1].
 *
 * @par Confirmed
 * Both axes clamp against the screen dimension globals before packing;
 * the 10-bit fields match the PSX draw-area command format.
 *
 * @par Likely role
 * The draw-area corner packer of the sprite builders (func_80093AF0,
 * func_8009398C, func_80093CE8); paired with func_80094010 (0xE4, the
 * bottom-right corner).
 *
 * @par Callers
 * func_8009398C, func_80093AF0, func_80093CE8.
 */
s32 func_80093F78(s16 x, s16 y)
{
    register s32 v1 __asm__("$3");
    s32 x_tagged;

    x = (x < 0) ? 0 : (((s16)D_8009DDF4 - 1 < x) ? D_8009DDF4 - 1 : x);
    y = (y < 0) ? 0 : (((s16)D_8009DDF6 - 1 < y) ? D_8009DDF6 - 1 : y);

    v1 = (y & 0x3FF) << 10;
    x_tagged = (x & 0x3FF) | 0xE3000000;
    return v1 | x_tagged;
}
