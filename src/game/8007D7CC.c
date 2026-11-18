// Categories: Graphics > HUD > Loadout Row | Graphics > Sprites > Atlas | Graphics > GPU Primitives > Gouraud Quad | Graphics > 2D Drawing > Text
#include "game/types.h"

typedef struct {
    s32 unk0;
    u8 *unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u16 unk12;
} Unk8009C0A8;

typedef struct {
    u8 pad0[8];
    s16 unk8;
    s16 unkA;
    u8 padC[4];
    s16 unk10;
    u8 pad12[8];
    s16 unk1A;
} UnkBox;

typedef struct {
    u8 pad0[6];
    s16 unk6;
} UnkFont;

extern s32 gGameClock[] __asm__("D_8009FA60");
extern u8 gHighlightShadeTable __asm__("D_8009F3F8");
extern Unk8009C0A8 gLoadoutTable[] __asm__("D_8009C0A8");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");

extern UnkBox *func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern UnkBox *func_80082B0C(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082EF0(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80083060(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern UnkFont *func_80048E88(void);
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);

/**
 * @brief Draws one row of the loadout/upgrade list: the entry's icon in a framed box,
 *        an optional capacity bar with a gauge, an over-limit corner marker and the
 *        entry's value as a formatted number.
 *
 * @param p Six-byte display record: byte 1 and byte 2 are atlas sprite indices (byte 2
 *        when sel is non-zero, byte 1 otherwise) and the half-word at +4 is the text id
 *        of the number.
 * @param x, y Top-left corner of the row.
 * @param flag Highlight flag; selects the brighter palette colours and enables the bar.
 * @param idx Index into D_8009C0A8, whose records are 20 bytes: a per-player value
 *        array at +4, the limit at +0xA and the bar sprite ids at +0xE, +0xF, +0x10,
 *        +0x11 and +0x12.
 * @param sel Selects which of the two sprite indices of the record is drawn.
 * @return Nothing; the row is skipped when the atlas lookup fails.
 *
 * @par Confirmed mechanics
 * The icon comes from func_80082AB0(sprite, x, y, -1, -1, 0x14, shade, 1, 8) - atlas
 * size, link 0x14 - where shade is 0x30 when the record's value for the local player
 * D_8009F8B8 exceeds the limit and 0x80 otherwise; that is the same comparison
 * func_8007DD08 performs as values[player] above limit. The frame is a gouraud
 * rectangle around the icon box, inset by 2 for a highlighted row and 1 otherwise,
 * linked at 0xBB8, in the palette colours D_8009F3F8[(D_8009FA60 >> 4) and 7] + 0x80
 * when highlighted and 0x60/0xC0/0xFF otherwise.
 *
 * @par Confirmed over-limit marker
 * When the value exceeds the limit, two L-shaped textured quads (func_80083060, code
 * 0xFF, thickness 2) mark the corners, one along the top edge and one along the bottom.
 *
 * @par Confirmed bar
 * Only while flagged and D_8009F468[5] differs from 2: the background sprite comes
 * from func_80082B0C with the ids at +0xF, +0x10 and +0x12, plus a second call with
 * +0x11 when that byte is set. The fill is then drawn as a gouraud rectangle from x
 * 0x7E to the box edge, a one pixel column of height |value - middle| + 1 at 0x7E, and
 * the remainder from 0x7E leftwards - all linked at 10.
 *
 * @par Confirmed footer
 * y drops by one and x advances by 4 plus the icon width; outside the mode-2 screen
 * x snaps to 0xC1 and y rises by 2, a small atlas sprite from the record's +0xE is
 * placed at (x + 1, y - 2) and func_800788F0 draws the number at y + font offset 6,
 * colour 0x10, one font size down and middle-aligned.
 *
 * @par Likely role
 * One weapon/upgrade slot of the loadout screen: icon, a box that lights up when the
 * slot holds the cursor, a bar comparing holdings against the limit, and the quantity.
 *
 * @par Unknown
 * Meaning of the byte at +0x11 and why the gauge origin is fixed at 0x7E.
 *
 * @par Callers
 * func_8007DD08, three times, for the record indices 0, 1 and 2.
 */
void gDrawLoadoutListRow(u8 *p, s32 x, s32 arg2, s32 flag, s32 idx, s32 sel) __asm__("func_8007D7CC");

void gDrawLoadoutListRow(u8 *p, s32 x, s32 arg2, s32 flag, s32 idx, s32 sel)
{
    s32 c0, c1, c2, c3;
    s32 c;
    s32 ci;
    s32 shade;
    u8 *pal;
    Unk8009C0A8 *e;
    register u32 base __asm__("$3");
    register UnkBox *box __asm__("$23");

    register s32 s1 __asm__("$17");
    register s32 s3 __asm__("$19");
    register s32 s6 __asm__("$22");
    register s32 s0 __asm__("$16");
    register s32 s2 __asm__("$18");
    register s32 s4 __asm__("$20");
    register s32 bx __asm__("$4");
    register s32 dx __asm__("$2");
    register s32 ay __asm__("$5");
    register s32 bw __asm__("$6");
    s32 ten;
    UnkFont *font;
    register s32 y __asm__("$21");

    y = arg2;
    ten = 10;

    if (flag) {
        c3 = 2;
        ci = gGameClock[0];
        __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)" : "=r"(pal));
        c0 = c1 = c2 = *(u8 *)(((ci >> 4) & 7) + (u32)pal) + 0x80;
    } else {
        c0 = 0x60;
        c1 = 0xC0;
        c2 = 0xFF;
        c3 = 1;
    }
    if (sel == 0) {
        c = p[2];
    } else {
        c = p[1];
    }
    base = (u32)gLoadoutTable;
    e = (Unk8009C0A8 *)((u32)(idx * 20) + base);
    __asm__ volatile("" : : "r"(e));
    shade = (e->unk4[gActivePlayerIndex] > e->unkA) ? 0x30 : 0x80;
    box = func_80082AB0(c, x, y, -1, -1, 0x14, shade, 1, 8);
    if (box != 0) {
        func_80082EF0(box->unk8 - c3, box->unkA - c3,
                      (box->unk10 - box->unk8) + c3 * 2,
                      (box->unk1A - box->unkA) + c3 * 2,
                      0xBB8, c0, c1, c2);
        if (e->unk4[gActivePlayerIndex] > e->unkA) {
            s1 = box->unk10;
            dx = box->unk8;
            s0 = box->unk1A;
            s1 -= dx;
            s1 = x + s1;
            s3 = s1 - 3;
            __asm__("" : "+r"(s0) : "r"(s3));
            s0 -= box->unkA;
            s0 = y + s0;
            s2 = s0 - 3;
            s6 = x + 3;
            s4 = 2;
            func_80083060(x, y + 3, s6, y, s3, s0, s1, s2, 0, 0xFF, 0, 0, s4);
            func_80083060(x, s2, s6, s0, s3, y, s1, y + 3, 0, 0xFF, 0, 0, s4);
        }
        if (flag && gFontBankTable[5] != 2) {
            s2 = (s32)func_80082B0C(e->unkF, e->unk10, e->unk12, -1, -1, 0x14, c0, c1, c2, 1, 8);
            if (e->unk11) {
                func_80082B0C(e->unkF, e->unk11, e->unk12, -1, -1, 0x14, c0, c1, c2, 1, 8);
            }
            if (s2 != 0) {
                s0 = ((UnkBox *)s2)->unkA;
                dx = ((UnkBox *)s2)->unk1A;
                s1 = box->unkA;
                base = (u32)(s32)box->unk1A;
                bx = ((UnkBox *)s2)->unk10;
                s0 = s0 + ((dx - s0) >> 1);
                ay = s0;
                bw = 0x7E - bx;
                __asm__("" : "+r"(s1) : "r"(ay), "r"(bw));
                s1 = s1 + (((s32)base - s1) >> 1);
                func_80082EF0(bx, ay, bw, 1, ten, 0x60, 0xC0, 0xFF);
                bx = 0x7E;
                __asm__("" : "+r"(s0) : "r"(bx));
                s0 = s0 - s1;
                dx = s0 >> 31;
                ay = (dx & s0) + s1;
                bw = 1;
                __asm__("" : "+r"(s0) : "r"(ay), "r"(bw));
                s0 = (dx + s0) ^ dx;
                func_80082EF0(bx, ay, bw, s0 + 1, ten, 0x60, 0xC0, 0xFF);
                dx = box->unk8;
                __asm__("" : "+r"(dx) : "r"(s1));
                func_80082EF0(0x7E, s1, box->unk8 - 0x7E - c3, 1, ten, 0x60, 0xC0, 0xFF);
            }
        }
        y -= 1;
        x += 4 + (box->unk10 - box->unk8);
    }
    if (gFontBankTable[5] != 2) {
        x = 0xC1;
        y += 2;
    }
    func_80082AB0(gLoadoutTable[idx].unkE, x + 1, y - 2, -1, -1, 0x14, 0x80, 1, 8);
    font = func_80048E88();
    func_800788F0(0, x, y + font->unk6, 0x10, *(u16 *)(p + 4), 1, 8, 2);
}
