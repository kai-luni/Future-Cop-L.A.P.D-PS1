#include "game/types.h"

struct Packet {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 x, y, w, h;
};
struct Page { u32 addr : 24; u32 len : 8; u32 code; };
extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;

void func_80026E9C(s32 r, s32 g, s32 b, s32 w)
{
    u8 *packet;
    if (D_8009FA90 < D_8009F9C0) goto page_alloc;
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
    if (D_8009FA90 < D_8009F9C0) goto rect_alloc;
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
