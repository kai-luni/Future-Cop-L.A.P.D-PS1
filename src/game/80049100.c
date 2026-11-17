// Categories: Graphics > Text > Rendering | Graphics > GPU Primitives
#include "game/types.h"

struct DrawTPage {
    u32 addr : 24;
    u32 len : 8;
    u32 code;
};

extern u8 *gPacketCursor[] __asm__("D_8009FA90");
extern u8 *gPacketLimit[] __asm__("D_8009F9C0");
extern u16 gTexpageTable[] __asm__("D_800A1940");
extern u16 gLoadedFont __asm__("D_8009F768");
extern s32 D_8009F76C;

extern s32 func_80048F98(s32 character, s32 x, s32 y, u16 color);

/**
 * @brief Draws a NUL-terminated string with the current font bank.
 *
 * @param text Character string.
 * @param x, y Start position.
 * @param color Packed colour forwarded per character.
 * @return Nothing; returns immediately when no font is loaded (D_8009F768 == 0).
 *
 * @par Confirmed
 * One texpage packet (D_800A1940[0] & 0x9FF | 0xE1000000) is emitted first, then
 * the loop takes each byte and calls func_80048F98(character, x, y, color) - the
 * per-glyph emitter. Characters below 0x16 trigger the newline/control branch
 * (`character - 10 < 0x16`).
 *
 * @par Likely role
 * The debug/HUD text renderer; the pool it draws into is the 2D one
 * (D_8009FA90/D_8009F9C0).
 */
void func_80049100(u8 *text, s32 x, s32 y, s32 color)
{
    struct DrawTPage *packet;
    u32 character;

    if (gLoadedFont == 0) {
        return;
    }
    packet = (struct DrawTPage *)gPacketCursor[0];
    if (gPacketCursor[0] < gPacketLimit[0]) {
        gPacketCursor[0] = (u8 *)packet + 0x1C;
    }
    packet->addr = 0;
    packet->len = 1;
    packet->code = (gTexpageTable[D_8009F76C] & 0x9FF) | 0xE1000000;
    while ((character = *text) != 0) {
        text++;
        if (character - 10 < 0x16) {
            color = character - 10;
        } else {
            x += func_80048F98(character, x, y, color);
        }
    }
}
