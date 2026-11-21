// Categories: Graphics > Text > Glyph Rendering | Graphics > GPU Primitives
#include "game/types.h"

struct LookupState {
    unsigned char pad_00[0x10];
    unsigned char *values;
};

struct Glyph {
    u8 b00;
    u8 b01;
    u8 b02;
    u8 b03;
    u8 b04;
    u8 b05;
    u8 b06;
    u8 b07;
    s8 b08;
    u8 b09;
    u8 b0A;
};

struct Sprite {
    u32 addr : 24;
    u8 len;
    u8 b04;
    u8 b05;
    u8 b06;
    u8 b07;
    u16 w08;
    u16 w0A;
    u8 b0C;
    u8 b0D;
    u16 w0E;
    u16 w10;
    u16 w12;
};

extern u8 *gPacketCursor __asm__("D_8009FA90");
extern u8 *gPacketLimit __asm__("D_8009F9C0");
extern u16 gGlyphTable[] __asm__("D_8009AAE0");
extern u8 gPaletteTable[] __asm__("D_800A1970");

extern struct LookupState *gFont __asm__("D_8009F35C");
extern u16 gLoadedFont __asm__("D_8009F768");
extern s32 gCurrentFontBankIndex __asm__("D_8009F76C");

extern u8 *func_800489C8(struct LookupState *state, unsigned int offset);
s32 gEmitFontGlyphSprite(u8 character, s32 x, s32 y, u16 color) __asm__("func_80048F98");


/**
 * @brief Emits one glyph sprite at a position.
 *
 * @param character Character code.
 * @param x, y Screen position.
 * @param color Packed colour.
 * @return The glyph's advance/width byte (glyph->b08).
 *
 * @par Confirmed
 * The glyph record comes from func_800489C8(D_8009F35C, character); when the 2D
 * pool D_8009FA90 is full the function returns the advance without drawing. The
 * sprite fields are filled from the glyph (b02..b0A) and the texpage table
 * D_8009AAE0, with the font bitmap at D_800A1970.
 *
 * @par Likely role
 * The per-character half of func_80049100; the returned byte is what the string
 * loop adds to x.
 *
 * @par Environment path
 * Looks up character in the current font glyph table, emits an 8x8 textured
 * sprite (SPRT, code 0x64) at (x, y) with the given palette color into the
 * ordering table, and returns the glyph advance width (called by
 * func_80049100).
 */
s32 gEmitFontGlyphSprite(u8 character, s32 x, s32 y, u16 color)
{
    struct Sprite *sprite;
    struct Glyph *glyph;
    register u8 *table __asm__("$3");
    register u32 base __asm__("$3");
    register u32 index __asm__("$4");
    register u32 code __asm__("$3");
    register u32 u __asm__("$3");
    register s32 index2 __asm__("$4");

    if (gPacketCursor >= gPacketLimit) {
        sprite = (struct Sprite *)gPacketCursor;
    } else {
        sprite = (struct Sprite *)gPacketCursor;
        gPacketCursor = (u8 *)sprite + 0x1C;
    }
    sprite->addr = 0;
    glyph = (struct Glyph *)func_800489C8(gFont, character);
    sprite->len = 4;
    __asm__ volatile ("" : : : "memory");
    code = 100;
    __asm__ volatile ("" : "=r" (code) : "0" (code));
    index = color;
    __asm__ volatile ("" : "=r" (index) : "0" (index));
    sprite->b07 = code;
    __asm__ volatile ("" : : : "memory");
    base = (u32)gGlyphTable + 0x5520;
    __asm__ volatile ("" : "=r" (base) : "0" (base));
    base = base - 0x5520;
    __asm__ volatile ("" : "=r" (base) : "0" (base));
    color = ((u16 *)base)[index];
    sprite->b04 = (color >> 8) & 0x7C;
    sprite->b05 = (color >> 3) & 0x7C;
    sprite->b06 = (color & 0x1F) << 2;
    u = glyph->b04;
    __asm__ volatile ("" : "=r" (u) : "0" (u));
    sprite->b0C = u;
    table = (u8 *)((u32)gPaletteTable - 0x1970);
    __asm__ volatile ("" : "=r" (table) : "0" (table));
    index2 = gCurrentFontBankIndex;
    __asm__ volatile ("" : "=r" (index2) : "0" (index2), "r" (table));
    table += 0x1970;
    __asm__ volatile ("" : "=r" (table) : "0" (table));
    sprite->b0D = glyph->b06 + table[index2 * 2];
    sprite->w10 = glyph->b02;
    sprite->w12 = glyph->b03;
    sprite->w08 = x + (s8)glyph->b09;
    sprite->w0A = y + (s8)glyph->b0A;
    sprite->w0E = gLoadedFont;
    return glyph->b08;
}
