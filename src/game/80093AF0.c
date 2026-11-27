// Categories: Graphics > GPU Primitives > Sprite | Graphics > 2D Drawing
#include "game/types.h"

extern u16 gMaxDrawWidth __asm__("D_8009DDF4");
extern u16 gMaxDrawHeight __asm__("D_8009DDF6");
extern s32 gBuildDrawAreaCommands(s16 x, s16 y) __asm__("func_80093F78");
extern s32 gPackDrawAreaBottomRight(s32 x, s32 y) __asm__("func_80094010");
extern u32 gPackDrawOffset(u32 x, u32 y) __asm__("func_800940A8");
extern u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage) __asm__("func_80093F58");
extern s32 gBuildTexWindowCommand(void *p) __asm__("func_800940C4");
void gBuildSpritePacket(u8 *arg0, u8 *arg1) __asm__("func_80093AF0");


/**
 * @brief Builds a GPU drawing-environment packet (DR_ENV) from a descriptor:
 *        clip area, draw offset, draw mode, tex window, mask, and optional clear.
 *
 * @param arg0 Output packet buffer; byte 3 receives the command word count (6 or 9).
 * @param arg1 Descriptor: +0/+2/+4/+6 clip x/y/w/h, +8/+0xA draw offset x/y,
 *        +0xC..+0x13 texture window Rect, +0x14 tpage, +0x16/+0x17 dtd/dfe,
 *        +0x18 clear-background flag, +0x19..+0x1B RGB.
 * @return Nothing.
 *
 * @par Confirmed
 * Words: +4 = func_80093F78(x, y) (GP0 0xE3), +8 = func_80094010(x+w-1, y+h-1)
 * (GP0 0xE4), +0xC = func_800940A8(ofsX, ofsY) (GP0 0xE5), +0x10 =
 * func_80093F58(dfe, dtd, tpage) (GP0 0xE1), +0x14 = func_800940C4(rect+0xC)
 * (GP0 0xE2), +0x18 = 0xE6000000 (GP0 0xE6 mask-bit). When rect[0x18] != 0,
 * appends three words for a GP0 0x60 flat-colour rect: 0x60000000 | RGB,
 * (x-ofsX, y-ofsY), and (w, h) clamped to [0, D_8009DDF4-1]/[0, D_8009DDF6-1].
 * obj[3] = count - 1 (6 without clear rect, 9 with clear rect).
 *
 * @par Likely role
 * Psy-Q SetDrawEnv packet builder: emits the standard GP0 E1..E6 state setup
 * commands and an optional GP0 0x60 background clear rectangle.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; dispatch unconfirmed (likely a packet-builder table).
 *
 * @par Environment path
 * Builds a GPU drawing-environment (DR_ENV) command packet from a descriptor.
 * Populates draw-area (func_80093F78, func_80094010), draw-offset
 * (func_800940A8), draw-mode (func_80093F58), texwindow (func_800940C4), and
 * 0xE6000000 mask commands, and when rect[0x18] is set appends a 0x60 clear
 * rectangle clamped to D_8009DDF4/D_8009DDF6.
 */
void gBuildSpritePacket(u8 *arg0, u8 *arg1)
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
        buf[0] -= *(u16 *)(rect + 8);
        buf[1] -= *(u16 *)(rect + 0xA);
        p[count++] = 0x60000000 | (rect[0x1B] << 16) | (rect[0x1A] << 8) | rect[0x19];
        p[count++] = *(u32 *)&buf[0];
        p[count++] = *(u32 *)&buf[2];
    }
    obj[3] = count - 1;
}
