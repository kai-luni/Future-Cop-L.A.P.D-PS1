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

extern u8 *D_8009FA90;
extern u8 *D_8009F9C0;
extern u16 D_8009AAE0[];
extern u8 D_800A1970[];

extern struct LookupState *D_8009F35C;
extern u16 D_8009F768;
extern s32 D_8009F76C;

extern u8 *func_800489C8(struct LookupState *state, unsigned int offset);

s32 func_80048F98(u8 character, s32 x, s32 y, u16 color)
{
    struct Sprite *sprite;
    struct Glyph *glyph;
    register u8 *table __asm__("$3");
    register u32 base __asm__("$3");
    register u32 index __asm__("$4");
    register u32 code __asm__("$3");
    register u32 u __asm__("$3");
    register s32 index2 __asm__("$4");

    if (D_8009FA90 >= D_8009F9C0) {
        sprite = (struct Sprite *)D_8009FA90;
    } else {
        sprite = (struct Sprite *)D_8009FA90;
        D_8009FA90 = (u8 *)sprite + 0x1C;
    }
    sprite->addr = 0;
    glyph = (struct Glyph *)func_800489C8(D_8009F35C, character);
    sprite->len = 4;
    __asm__ volatile ("" : : : "memory");
    code = 100;
    __asm__ volatile ("" : "=r" (code) : "0" (code));
    index = color;
    __asm__ volatile ("" : "=r" (index) : "0" (index));
    sprite->b07 = code;
    __asm__ volatile ("" : : : "memory");
    base = (u32)D_8009AAE0 + 0x5520;
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
    table = (u8 *)((u32)D_800A1970 - 0x1970);
    __asm__ volatile ("" : "=r" (table) : "0" (table));
    index2 = D_8009F76C;
    __asm__ volatile ("" : "=r" (index2) : "0" (index2), "r" (table));
    table += 0x1970;
    __asm__ volatile ("" : "=r" (table) : "0" (table));
    sprite->b0D = glyph->b06 + table[index2 * 2];
    sprite->w10 = glyph->b02;
    sprite->w12 = glyph->b03;
    sprite->w08 = x + (s8)glyph->b09;
    sprite->w0A = y + (s8)glyph->b0A;
    sprite->w0E = D_8009F768;
    return glyph->b08;
}
