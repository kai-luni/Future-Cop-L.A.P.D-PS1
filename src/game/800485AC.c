// Categories: Graphics > HUD > Frame | Graphics > 2D Drawing
#include "game/types.h"

struct DrawTPage {
    u32 addr : 24;
    u32 len : 8;
    u32 code;
};

struct Sprt {
    u32 addr : 24;
    u32 len : 8;
    u8 r;
    u8 g;
    u8 b;
    u8 code;
    s16 x;
    s16 y;
    u8 u;
    u8 v;
    u16 pad;
    u16 w;
    u16 h;
};

extern s32 D_8009FAC8;
extern s32 D_8009F9B0[];
extern s32 gPlayerCount[] __asm__("D_8009FA0C");
extern u8 *D_8009FA90;
extern u8 *gPacketLimit __asm__("D_8009F9C0");

/**
 * @brief Draws the HUD frame / border for the current view mode.
 *
 * @param value Mode selector; 1 selects the wide layout.
 * @return Nothing; returns early when the frame switch D_8009FAC8 is 0 or a frame
 *         is already drawn (D_8009F9B0[0] != 0).
 *
 * @par Confirmed
 * Two packets come from D_8009FA90 (guarded by D_8009F9C0) - a texpage word
 * 0xE100011F plus the border primitive - with the fixed coordinates 0x30, 0x40,
 * 0x64, 0x80, 0xB2, 0xCC and 0xF4, i.e. the static layout of the frame. Which
 * variant is used depends on D_8009FA0C[0] == 1 and on @c value.
 *
 * @par Likely role
 * The frame around the play view (letterbox plus side panels).
 */
void func_800485AC(s32 value)
{
    u8 *packet;
    register s32 arg __asm__("$7") = value;
    s32 x;
    s32 w;

    value = 0;

    if (D_8009FAC8 == 0) {
        return;
    }
    if (D_8009F9B0[0] != 0) {
        return;
    }

    x = 0xF4;
    if (gPlayerCount[0] != 1) {
        x = (arg == 0) << 7;
    }

    if (D_8009FA90 >= gPacketLimit) {
        packet = D_8009FA90;
        goto page_ready;
    }
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
page_ready:
    ((struct DrawTPage *)packet)->addr = 0;
    ((struct DrawTPage *)packet)->len = 1;
    ((struct DrawTPage *)packet)->code = 0xE100011F;

    if (D_8009FA90 >= gPacketLimit) {
        packet = D_8009FA90;
        goto sprt_ready;
    }
    packet = D_8009FA90;
    D_8009FA90 = packet + 0x1C;
sprt_ready:

    ((struct Sprt *)packet)->code = 0x64;
    ((struct Sprt *)packet)->r = 0x80;
    ((struct Sprt *)packet)->g = 0x80;
    ((struct Sprt *)packet)->b = 0x80;
    ((struct Sprt *)packet)->addr = 0;
    ((struct Sprt *)packet)->len = 4;

    if (gPlayerCount[0] == 1) {
        ((struct Sprt *)packet)->u = 0;
        w = 0x40;
    } else {
        ((struct Sprt *)packet)->u = 0;
        if (arg == 1) {
            ((struct Sprt *)packet)->u = 0x20;
        }
        w = 0x20;
    }
    ((struct Sprt *)packet)->w = w;
    ((struct Sprt *)packet)->v = 0xCC;
    ((struct Sprt *)packet)->h = 0x30;
    ((struct Sprt *)packet)->x = x;
    ((struct Sprt *)packet)->y = 0xB2;
}
