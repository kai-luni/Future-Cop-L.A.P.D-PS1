// Categories: Graphics > Text > Rendering | Graphics > Text > Font Upload
#include "game/types.h"

struct FontEntry {
    s16 f00;
    s16 f02;
    s16 f04;
    s16 f06;
    s16 f08;
    union {
        u16 u;
        s16 s;
    } f0A;
    s16 f0C;
    s16 f0E;
    s32 f10;
    s32 f14;
    s32 f18;
    u8 data[0x100];
};

struct FontSource {
    u8 pad00[0xA];
    u16 count;
    u8 pad0C[6];
    u8 f12;
    u8 f13;
    s32 off14;
    s32 pad18;
    s32 off1C;
};

struct FontInfo {
    u8 pad00[4];
    s16 f04;
};

extern struct FontEntry gGlyphEntryTable[] __asm__("D_800A19A0");
extern s32 gFontBankCount __asm__("D_8009F364");

extern s32 func_800489EC(unsigned char *value);
extern void gLoadFontAsset(s32 index) __asm__("func_80048BF8");
extern s32 gSelectFontBank(s32 index) __asm__("func_80048E28");
void gRegisterFontBank(s32 arg0, s32 *arg1) __asm__("func_80048A14");


/**
 * @brief Registers a font in the next free slot (max four): builds the
 *        character map, width statistics and row pitch, then uploads it.
 *
 * @param arg0 Font source header (count, glyph table and info offsets).
 * @param arg1 Owner pointer stored in the slot.
 * @return Nothing; silently returns when all four slots are taken.
 *
 * @par Confirmed
 * Slot = D_8009F364 (>= 4 aborts). The entry D_800A19A0[slot] receives
 * f18 = arg1, f02/f04 = sign-extended bytes f12/f13 of the source,
 * f14 = src + off1C (info), f10 = src + off14 (glyph table), f0C = count.
 * Per glyph (11-byte stride): data[charset] = index, f0A accumulates the
 * width byte, f08/f06 track max width/height. f0A becomes the average
 * width, f00 = func_800489EC(info), f0E = ((info->f04 * f00 + 0x1F) &
 * ~0x1F) >> 3 (32-bit-aligned row bytes). func_80048BF8 uploads the slot,
 * the counter increments and slot 0 is selected via func_80048E28(0)
 * when it was the first font.
 *
 * @par Callers
 * func_80048E9C.
 */
void gRegisterFontBank(s32 arg0, s32 *arg1)
{
    register struct FontSource *src __asm__("$7");
    struct FontEntry *e;
    struct FontInfo *info;
    u8 *p;
    register s32 i __asm__("$6");
    s32 positive;
    s32 off;
    u16 cnt;
    s32 off14;

    src = (struct FontSource *)arg0;
    if (gFontBankCount >= 4) {
        return;
    }

    e = &gGlyphEntryTable[gFontBankCount];
    off = src->off1C;
    e->f18 = (s32)arg1;
    e->f02 = (s16)(s8)src->f12;
    e->f04 = (s16)(s8)src->f13;
    e->f06 = 0;
    cnt = src->count;
    info = (struct FontInfo *)((u8 *)src + off);
    e->f14 = (s32)info;
    e->f0C = (s16)cnt;
    off14 = src->off14;
    e->f08 = 0;
    e->f0A.u = 0;
    e->f10 = (s32)((u8 *)src + off14);

    __asm__("" : : "r"(src) : "memory");
    p = (u8 *)e->f10;
    positive = e->f0C > 0;
    __asm__("" : "=r"(positive) : "0"(positive));
    if (positive) {
        i = 0;
        do {
            e->data[p[0]] = (u8)i;
            e->f0A.u = e->f0A.u + p[2];
            if ((s16)p[2] > e->f08) {
                __asm__("" : : : "memory");
            e->f08 = (s16)p[2];
            }
            if ((s16)p[3] > e->f06) {
                __asm__("" : : : "memory");
            e->f06 = (s16)p[3];
            }
            p += 11;
            i++;
        } while (i < e->f0C);
    }

    e->f0A.s = (s16)(e->f0A.s / e->f0C);
    e->f00 = (s16)func_800489EC((u8 *)info);
    e->f0E = (s16)((((info->f04 * func_800489EC((u8 *)info)) + 0x1F) & ~0x1F) >> 3);

    gLoadFontAsset(gFontBankCount);
    gFontBankCount++;
    if (gFontBankCount == 1) {
        gSelectFontBank(0);
    }
}
