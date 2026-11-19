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
 * @brief Builds a GPU sprite packet from a rectangle descriptor: mode,
 *        corners, UV, tpage/clut words and an optional flat-colour rect.
 *
 * @param obj Output packet buffer; byte 3 receives the word count - 1.
 * @param rect Descriptor: +0/+2 x/y, +4/+6 w/h, +8/+0xA UV, +0xC clut,
 *        +0x14 tpage, +0x16/+0x17 dtd/dfe, +0x18 flat-rect flag,
 *        +0x19..+0x1B RGB.
 * @return Nothing.
 *
 * @par Confirmed
 * Words: +4 = func_80093F78(x, y), +8 = func_80094010(x+w-1, y+h-1),
 * +0xC = func_800940A8(u, v), +0x10 = func_80093F58(dfe, dtd, tpage),
 * +0x14 = func_800940C4(clut), +0x18 = 0xE6000000 (draw-mode reset).
 * With the flat flag: three more words 0x60000000 | RGB, the position
 * minus UV and the size clamped to D_8009DDF4-1/D_8009DDF6-1 (the screen
 * bounds); the base count is 7.
 *
 * @par Likely role
 * One of the packet builders of the 2D layer: the 0xE6 reset word and
 * the 0x60 flat-rect opcode are standard PSX GPU commands, the clamping
 * keeps off-screen sprites inside the draw area.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; dispatch unconfirmed (likely a packet-builder table).
 *
 * @par Environment path
 * Builds a 2D GPU sprite command packet from a rectangle descriptor.
 * Populates draw-area (func_80093F78, func_80094010), texcoord
 * (func_800940A8), draw-mode (func_80093F58), texwindow (func_800940C4), and
 * 0xE6000000 commands, and when rect[0x18] is set appends a 0x60 primitive
 * clamped to D_8009DDF4/D_8009DDF6.
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
