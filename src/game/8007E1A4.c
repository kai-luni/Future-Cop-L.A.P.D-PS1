// Categories: Game Engine > Front-End Configuration | Graphics > HUD > Options | Graphics > 2D Drawing > Text
#include "game/types.h"

struct E8007E1A4 {
    u8 unk00[4];
    s32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u16 unkC;
    u8 unk0E[6];
};

extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern s8 gPanelSyncValue __asm__("D_8009F8BC");
extern struct E8007E1A4 gLoadoutTable[] __asm__("D_8009C0A8");
extern u8 gNumberFormatString[] __asm__("D_8009F490");

extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern s32 func_80048E28(s32 index);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern u8 *gLookupTextString(s32 id) __asm__("func_80089914");
extern void func_8008CD30(u8 *dest, u8 *fmt, ...);
extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                          s32 alignX, s32 alignY, s32 font);

/**
 * @brief Options-row widget: steps a per-player setting byte with wraparound
 *        (modes 1/2) or draws the current value as a fraction label (mode 0).
 *
 * @param mode 0 draw, 1 increment, 2 decrement.
 * @param x, y Label position (mode 0).
 * @param color Text colour (mode 0).
 * @param idx 1-based row index; player 1 addresses row idx - 5, player 0
 *        row idx - 1 in the 20-byte record table D_8009C0A8.
 * @return Nothing.
 *
 * @par Confirmed
 * The record's +4 pointer holds one setting byte per player. Modes 1/2 play
 * sound 0x46 and step the byte, wrapping at the record's limits +9 (min)
 * and +8 (count): increment wraps to min at count, decrement wraps to
 * count - 1 below min. Mode 0 reads the neighbouring row b, computes
 * n = unkA - unk9 + 1, and when n >= 2 fetches the label text
 * func_80089914(unkC); unless the font is already 2 it switches to font 2
 * via func_80048E28(2), measures the text with func_800492A0, formats
 * func_8008CD30(buf, D_8009F490, n) and draws it at (x + width, y) in the
 * given colour, alignX 1, size 8, font 2, then restores the old font.
 *
 * @par Likely role
 * One row of the options/configuration screen, sibling of the panel
 * func_800260B8: the byte is the selected option, the label shows
 * "value of n" in the small font.
 *
 * @par Unknown
 * The format string behind D_8009F490 and why player 1 offsets by 5.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely reached through a runtime-built widget table (unconfirmed).
 */
void gStepOptionsSetting(s32 mode, s32 x, s32 y, s32 color, s32 idx) __asm__("func_8007E1A4");

void gStepOptionsSetting(s32 mode, s32 x, s32 y, s32 color, s32 idx)
{
    struct E8007E1A4 *e;
    struct E8007E1A4 *e2;
    u8 *base;
    u8 *p;
    u8 *s;
    s32 n;
    s32 old;
    s32 w;
    s32 h;
    s32 t;
    s32 b;
    s32 a;
    u8 buf[16];

    t = gFontBankTable[gActivePlayerIndex];
    a = t - 1;
    b = idx - 1;
    if (gActivePlayerIndex == 1) {
        a = t - 5;
        b = idx - 5;
    }
    base = (u8 *)gLoadoutTable;
    e = (struct E8007E1A4 *)(base + a * 20);
    p = (u8 *)e->unk4;
    switch (mode) {
    case 2:
        gPlayNonpositionalSoundEffect(0x46, 0);
        p[gActivePlayerIndex]--;
        if (((s8 *)p)[gActivePlayerIndex] < e->unk9) {
            p[gActivePlayerIndex] = e->unk8 - 1;
        }
        break;
    case 1:
        gPlayNonpositionalSoundEffect(0x46, 0);
        p[gActivePlayerIndex]++;
        if (((s8 *)p)[gActivePlayerIndex] >= e->unk8) {
            p[gActivePlayerIndex] = e->unk9;
        }
        break;
    case 0:
        e2 = &gLoadoutTable[b];
        n = e2->unkA - e2->unk9 + 1;
        if (n >= 2) {
            s = gLookupTextString(e2->unkC);
            if (s != 0) {
                old = -1;
                if (gPanelSyncValue != 2) {
                    old = gPanelSyncValue;
                    func_80048E28(2);
                }
                func_800492A0(s, &w, &h);
                func_8008CD30(buf, gNumberFormatString, n);
                func_800788F0(buf, x + w, y, color, 0, 1, 8, 2);
                if (old != -1) {
                    func_80048E28(old);
                }
            }
        }
        break;
    }
}
