#include "game/types.h"

struct HalfRect { u32 xy, wh; };
struct DrawTPage { u32 addr : 24; u32 len : 8; u32 code; };
struct Packet {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    struct HalfRect rect;
};
extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;

void func_8001C148(u8 red, u8 green, u8 blue, struct HalfRect *rect, u8 blend)
{
    u8 *packet;
    if (D_8009FA90 < D_8009F9C0) {
        packet = D_8009FA90;
        D_8009FA90 = packet + 0x1C;
    } else {
        packet = D_8009FA90;
    }
page_ready:
    ((volatile struct DrawTPage *)packet)->addr = 0;
    ((volatile struct DrawTPage *)packet)->len = 1;
    ((struct DrawTPage *)packet)->code = 0xE1000000;
    if (D_8009FA90 < D_8009F9C0) goto rect_alloc;
    __asm__("");
    packet = D_8009FA90;
    goto rect_ready;
rect_alloc:
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
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
