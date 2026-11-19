// Categories: Platform > GPU | Graphics > GPU Command Lists | Graphics > 2D Drawing
#include "game/types.h"

extern u16 gMaxDrawWidth __asm__("D_8009DDF4");
extern u16 gMaxDrawHeight __asm__("D_8009DDF6");
extern s32 gBuildDrawAreaCommands(s16 x, s16 y) __asm__("func_80093F78");
extern s32 gPackDrawAreaBottomRight(s32 x, s32 y) __asm__("func_80094010");
extern u32 gPackDrawOffset(u32 x, u32 y) __asm__("func_800940A8");
extern u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage) __asm__("func_80093F58");
extern s32 gBuildTexWindowCommand(void *p) __asm__("func_800940C4");
void gBuildRectDrawList(u8 *arg0, u8 *arg1) __asm__("func_80093CE8");


/**
 * @brief Builds a six-command GPU display list for a rectangle descriptor,
 *        plus an optional fill/textured-quad primitive.
 *
 * @param obj Output command buffer; words 4..0x18 receive the commands,
 *        byte 3 the command count minus one.
 * @param rect Descriptor: +0/+2 the x/y origin, +4/+6 width/height, +8/+0xA
 *        the texture u/v, +0xC the 8-byte texture window, +0x14 tpage,
 *        +0x16 dtd, +0x17 dfe, +0x18 a fill-enable byte, +0x19..+0x1B RGB.
 * @return Nothing.
 *
 * @par Confirmed
 * The fixed commands: func_80093F78(x, y), func_80094010(x + w - 1,
 * y + h - 1), func_800940A8(u, v), func_80093F58(dfe, dtd, tpage) (E1h
 * draw mode), func_800940C4(rect + 0xC) (E2h texture window) and
 * 0xE6000000 (E6h mask reset). With rect[0x18] != 0 the width/height are
 * clamped to [0, D_8009DDF4 - 1]/[0, D_8009DDF6 - 1]; when x or w is not
 * 64-aligned (& 0x3F) a 0x60 textured rectangle is emitted with the origin
 * shifted by (u, v), otherwise a 0x02 VRAM fill rectangle; both carry the
 * RGB bytes and the clamped size pair.
 *
 * @par Likely role
 * The panel/sprite list builder of the GPU command paths func_800932F0
 * and func_800933B0: aligned rectangles take the fast VRAM fill, unaligned
 * ones fall back to a textured quad with the window offset compensated.
 *
 * @par Unknown
 * The exact semantics of the E6h mask reset in this pipeline.
 *
 * @par Callers
 * func_800932F0, func_800933B0.
 *
 * @par Environment path
 * Builds a GPU display list from a rectangle descriptor for func_800932F0 and
 * func_800933B0. Writes draw-area (func_80093F78, func_80094010), texcoord
 * (func_800940A8), draw-mode (func_80093F58), texwindow (func_800940C4), and
 * 0xE6000000 commands, then if enabled clamps size to D_8009DDF4/D_8009DDF6
 * and appends a 0x60 sprite or 64-aligned 0x02 fill rect.
 */
void gBuildRectDrawList(u8 *arg0, u8 *arg1)
{
    u8 *rect = arg1;
    u8 *obj = arg0;

    u16 buf[4];
    u32 *p;
    s32 count;

    *(s32 *)(obj + 4) = gBuildDrawAreaCommands(*(s16 *)(rect + 0), *(s16 *)(rect + 2));
    *(s32 *)(obj + 8) = gPackDrawAreaBottomRight(
        (s16)(*(u16 *)(rect + 4) + *(u16 *)(rect + 0) - 1),
        (s16)(*(u16 *)(rect + 2) + *(u16 *)(rect + 6) - 1));
    *(s32 *)(obj + 0xC) = gPackDrawOffset(*(s16 *)(rect + 8), *(s16 *)(rect + 0xA));
    *(s32 *)(obj + 0x10) = gBuildDrawModeCommand(*(u8 *)(rect + 0x17), *(u8 *)(rect + 0x16), *(u16 *)(rect + 0x14));
    *(s32 *)(obj + 0x14) = gBuildTexWindowCommand(rect + 0xC);
    *(s32 *)(obj + 0x18) = 0xE6000000;

    count = 7;
    if (rect[0x18] != 0) {
        buf[0] = *(u16 *)(rect + 0);
        buf[1] = *(u16 *)(rect + 2);
        buf[2] = *(u16 *)(rect + 4);
        buf[3] = *(u16 *)(rect + 6);

        buf[2] = (s16)buf[2] < 0 ? 0 : ((s16)gMaxDrawWidth - 1 < (s16)buf[2] ? gMaxDrawWidth - 1 : buf[2]);
        buf[3] = (s16)buf[3] < 0 ? 0 : ((s16)gMaxDrawHeight - 1 < (s16)buf[3] ? gMaxDrawHeight - 1 : buf[3]);

        p = (u32 *)obj;
        if ((buf[0] & 0x3F) != 0 || (buf[2] & 0x3F) != 0) {
            buf[0] -= *(u16 *)(rect + 8);
            buf[1] -= *(u16 *)(rect + 0xA);
            p[count++] = 0x60000000 | (rect[0x1B] << 16) | (rect[0x1A] << 8) | rect[0x19];
            p[count++] = *(u32 *)&buf[0];
            p[count++] = *(u32 *)&buf[2];
        } else {
            p[count++] = 0x02000000 | (rect[0x1B] << 16) | (rect[0x1A] << 8) | rect[0x19];
            p[count++] = *(u32 *)&buf[0];
            p[count++] = *(u32 *)&buf[2];
        }
    }
    obj[3] = count - 1;
}
