// Categories: Graphics > 2D Drawing > Rectangles | Graphics > GPU Primitives
#include "game/types.h"

struct Packet {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 x, y, w, h;
};
struct Page { u32 addr : 24; u32 len : 8; u32 code; };
extern u8 *D_8009FA90;
extern u8 *gPacketLimit __asm__("D_8009F9C0");

/**
 * @brief Emits a full-height colour bar: texpage word plus a 0x62 rectangle.
 *
 * @param r, g, b Colour components.
 * @param w Width of the bar; the height is fixed at 0xF0 (240 lines).
 * @return Nothing; two primitives from the D_8009FA90 pool.
 *
 * @par Confirmed
 * Primitive 1: length 1, code 0xE10006A0 (a fixed texture page / draw mode).
 * Primitive 2: length 3, code 0x62 (semi-transparent), colour bytes, the xy word
 * zeroed and the width written, height 0xF0.
 *
 * @par Likely role
 * The side bars of the split-screen or the letterbox fill of the front-end.
 *
 * @par Callers
 * func_80026E9C is called from the 2D overlay paths (the same family as
 * func_8001C214).
 */
void func_80026E9C(s32 r, s32 g, s32 b, s32 w)
{
    u8 *packet;
    if (D_8009FA90 < gPacketLimit) goto page_alloc;
    __asm__("");
    packet = D_8009FA90;
    goto page_ready;
page_alloc:
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
page_ready:
    ((struct Page *)packet)->len = 1;
    ((struct Page *)packet)->code = 0xE10006A0;
    ((struct Page *)packet)->addr = 0;
    if (D_8009FA90 < gPacketLimit) goto rect_alloc;
    __asm__("");
    packet = D_8009FA90;
    goto rect_ready;
rect_alloc:
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
rect_ready:
    ((struct Packet *)packet)->len = 3;
    ((struct Packet *)packet)->code = 0x62;
    ((struct Packet *)packet)->h = 0xF0;
    ((struct Packet *)packet)->r = r;
    ((struct Packet *)packet)->g = g;
    ((struct Packet *)packet)->b = b;
    *(u32 *)(packet + 8) = 0;
    ((struct Packet *)packet)->w = w;
    ((struct Packet *)packet)->addr = 0;
}
