// Categories: Graphics > Primitives > Textured Quad | Graphics > Effects
#include "game/types.h"

struct Packet {
    u32 tag;
    u32 code;
    u32 xy0;
    s16 u0, v0;
    u32 xy1;
    s16 u1, v1;
    u32 xy2;
    s16 u2, v2;
    u32 xy3;
    s16 u3, v3;
};
extern u8 *D_8009F9F0;
extern u8 *D_8009FA28;

/**
 * @brief Emits a textured, semi-transparent quad with a computed texture window.
 *
 * @param x Texture window base (three of the four UV coordinates are derived
 *          from it: x, x + 0x1F, x + 0xB00, x + 0xB1F).
 * @param points Four vertices (16-bit pairs at +0, +8, +24, +32).
 * @param flags Quadrant/validity flags; the packet is only emitted when the four
 *        bytes are without bit 0 and equal 0xC0.
 * @return Nothing.
 *
 * @par Confirmed
 * The packet uses code 0x2C808080 (semi-transparent, 50 % grey), v1 = 0x11F and
 * v0 = 0, and the ordering-table tag is the average of the four vertex
 * coordinates minus 0x180, masked to 24 bits.
 *
 * @par Likely role
 * The projected-quad path of the effects renderer: the flags encode which of the
 * four quadrants a face is in, and 0xC0 means "the whole quad is on screen".
 *
 * @par Callers
 * func_80042AF4 (the effect manager).
 */
void func_80042004(s32 x, u8 *points, u8 *flags)
{
    u32 f;
    struct Packet *p;
    s32 a, b, c, d;
    register u32 mask __asm__("$8");
    f = flags[0] | flags[1] | flags[4] | flags[3];
    if (!(f & 1) && f == 0xc0) {
        if (D_8009F9F0 >= D_8009FA28) {
            p = (struct Packet *)D_8009F9F0;
            __asm__ volatile ("");
            goto ready;
        }
        p = (struct Packet *)D_8009F9F0;
        D_8009F9F0 = (u8 *)p + 0x34;
ready:
        mask = 0xffffff;
        a = *(s16 *)(points+4);
        b = *(s16 *)(points+12);
        c = *(s16 *)(points+36);
        d = *(s16 *)(points+28);
        p->code = 0x2c808080;
        p->v1 = 0x11f;
        p->v0 = 0;
        p->tag = (p->tag & 0xff000000) | ((((a + b + c + d) >> 2) - 0x180) & mask);
        ((u8 *)p)[3] = 9;
        p->xy0 = *(u32 *)(points+0);
        p->xy1 = *(u32 *)(points+8);
        p->xy3 = *(u32 *)(points+32);
        p->xy2 = *(u32 *)(points+24);
        p->u1 = x + 0x1f;
        p->u2 = x + 0xb00;
        p->u0 = x;
        p->u3 = x + 0xb1f;
    }
}
