// Categories: Graphics > 2D Drawing > Rectangles | Graphics > GPU Primitives
#include "game/types.h"

struct HalfRect { u32 xy, wh; };
struct DrawTPage { u32 addr : 24; u32 len : 8; u32 code; };
struct Packet {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    struct HalfRect rect;
};
extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");

/**
 * @brief Emits a filled rectangle: a texpage word plus a 24-bit colour rectangle.
 *
 * @param red, green, blue Colour components.
 * @param rect Rectangle (packed xy and wh words).
 * @param blend Non-zero switches the primitive from opaque (0x60) to
 *              semi-transparent (0x62).
 * @return Nothing; two 28-byte primitives are taken from the D_8009FA90 pool
 *         (falling back to the last slot when it is exhausted).
 *
 * @par Confirmed contents
 * Primitive 1: length 1, code 0xE1000000 (draw-mode/texpage).
 * Primitive 2: length 3, code 0x60 (or 0x62 when blending), the colour bytes and
 * the rectangle's xy/wh.
 *
 * @par Callers
 * func_8001C214 (the blended overlay rectangle) and the 2D screens.
 */
void func_8001C148(u8 red, u8 green, u8 blue, struct HalfRect *rect, u8 blend)
{
    u8 *packet;
    if (gPacketCursor < gPacketLimit) {
        packet = gPacketCursor;
        gPacketCursor = packet + 0x1C;
    } else {
        packet = gPacketCursor;
    }
page_ready:
    ((volatile struct DrawTPage *)packet)->addr = 0;
    ((volatile struct DrawTPage *)packet)->len = 1;
    ((struct DrawTPage *)packet)->code = 0xE1000000;
    if (gPacketCursor < gPacketLimit) goto rect_alloc;
    __asm__("");
    packet = gPacketCursor;
    goto rect_ready;
rect_alloc:
    packet = gPacketCursor;
    gPacketCursor = packet + 0x1C;
rect_ready:
    ((struct Packet *)packet)->code = 0x60;
    ((struct Packet *)packet)->addr = 0;
    ((struct Packet *)packet)->len = 3;
    if (blend) ((struct Packet *)packet)->code = 0x62;
    ((struct Packet *)packet)->r = red;
    ((struct Packet *)packet)->g = green;
    ((struct Packet *)packet)->b = blue;
    ((struct Packet *)packet)->rect.xy = rect->xy;
    ((struct Packet *)packet)->rect.wh = rect->wh;
}
