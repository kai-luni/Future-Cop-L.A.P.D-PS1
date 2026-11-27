// Categories: Graphics > GPU Primitives > Draw Area
#include "game/types.h"

extern s16 gMaxDrawWidth __asm__("D_8009DDF4");
extern s16 gMaxDrawHeight __asm__("D_8009DDF6");
s32 gPackDrawAreaBottomRight(s32 x, s32 y) __asm__("func_80094010");


/**
 * @brief Builds the GPU draw-area bottom-right word (opcode 0xE4) with
 *        screen clamping.
 *
 * @param x, y Corner coordinates.
 * @return 0xE4000000 | (y & 0x3FF) << 10 | (x & 0x3FF), clamped like
 *         func_80093F78.
 *
 * @par Confirmed
 * Same clamping against D_8009DDF4/D_8009DDF6 as the 0xE3 packer, written
 * with explicit branch chains instead of ternaries (the compiler shape
 * differs, the semantics match).
 *
 * @par Likely role
 * The second draw-area corner of the sprite packet builders; together the
 * 0xE3/0xE4 pair sets the GPU clipping rectangle.
 *
 * @par Callers
 * func_8009398C, func_80093AF0, func_80093CE8.
 *
 * @par Environment path
 * Builds the GP0 E4 drawing-area bottom-right word: x and y are clamped to
 * 0..gMaxDrawWidth - 1 and 0..gMaxDrawHeight - 1 and packed into bits 0-9 and
 * 10-19. It is used by the draw-environment packet builders func_8009398C,
 * func_80093AF0 and func_80093CE8 to close the GPU clip rectangle.
 */
s32 gPackDrawAreaBottomRight(s32 x, s32 y)
{
    register s32 cx __asm__("$2");
    register s32 py __asm__("$3");
    s32 px;
    u16 maxw;
    u16 maxh;

    if ((s16)x >= 0) {
        maxw = *(u16 *)&gMaxDrawWidth;
        if ((s16)x > gMaxDrawWidth - 1) {
            cx = maxw - 1;
        } else {
            cx = x;
        }
    } else {
        cx = 0;
    }
    x = cx;

    if ((s16)y >= 0) {
        maxh = *(u16 *)&gMaxDrawHeight;
        if ((s16)y > gMaxDrawHeight - 1) {
            y = maxh - 1;
        }
    } else {
        y = 0;
    }

    py = (y & 0x3FF) << 10;
    px = (x & 0x3FF) | 0xE4000000;
    return py | px;
}
