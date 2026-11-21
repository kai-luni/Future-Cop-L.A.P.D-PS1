// Categories: Graphics > HUD > Name Entry | Graphics > 2D Drawing > Text | Utilities > Character Table | Game Engine > Front-End Configuration
#include "game/types.h"

struct FontEntry {
    u8 unk00[6];
    s16 height;
    s16 unk8;
};

extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 *gNameEntryAlphabet __asm__("D_8009F878");
extern s16 gPlayerNameLength __asm__("D_8009F87C");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern u8 gPlayerNameTable[] __asm__("D_800A6FE0");
extern u8 gNameEntryGlyphBuf[] __asm__("D_800A3928");
extern s32 gScalarCursor[] __asm__("D_8009F8F8");
extern u8 gHighlightShadeTable[8] __asm__("D_8009F3F8");
extern s32 gGameClock[] __asm__("D_8009FA60");

extern void gDecodeGlyphsToName(u8 *dst, u8 *src, s32 len, u8 *table) __asm__("func_8007AA9C");
extern void gEncodeNameToGlyphs(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen) __asm__("func_8007A9D0");
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern struct FontEntry *gGetActiveFont(void) __asm__("func_80048E88");
extern void gMeasureTextExtent(u8 *text, s32 *width, s32 *height) __asm__("func_800492A0");
extern void gEmitFlatQuadRect(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b) __asm__("func_80082EF0");
extern void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");

/**
 * @brief Name-entry widget for one player: handles the action codes of the character
 *        grid (toggle editing, move the cursor, step the glyph) and draws either the
 *        grid with the selected cell framed, or the plain name.
 *
 * @param action 0 draw, 1/2 move the glyph cursor, 3 toggle the edit session, 4/5 step
 *        the glyph under the cursor.
 * @param baseX x position; while the grid is active it is first shifted left by four
 *        cell advances so the eight cells are centred on it.
 * @param baseY y position; the text baseline is baseY + 10.
 * @return Nothing.
 *
 * @par Confirmed state
 * The player index is D_8009F8B8 and every field is per player: the edit flag
 * D_8009F8C0[idx], the cursor word D_8009F8F8[idx], the eight glyph indices at
 * D_800A3928 + idx * 8 and the name text at D_800A6FE0 + idx * 9. Glyph indices map
 * through the sixty-four entry table D_8009F878.
 *
 * @par Confirmed actions
 * 3: leaving an edit session decodes the indices back into the name with func_8007AA9C
 * and clears the flag; entering one sets the flag, zeroes the cursor and encodes the
 * name with func_8007A9D0 using the table length D_8009F87C.
 * 4/5: glyph index in 0..D_8009F87C - 1 with wraparound, plus sound
 * func_80046C84(0x46, 0). 1/2: cursor in 0..7 with wraparound, plus sound
 * func_80046C84(0x47, 0). All three are gated on the edit flag being set.
 *
 * @par Confirmed drawing
 * Active: the cell advance is the font field at +0x8, raised by one while
 * D_8009F468[5] equals 1, and each glyph width comes from func_800492A0, so the glyph
 * is centred inside its cell. The cell under the cursor gets a gouraud box
 * (func_80082EF0, link 10) of the cell advance by 2 at y + font height + 2, coloured
 * from D_8009F3F8[(D_8009FA60[0] >> 4) and 7] + 0x80, and its glyph is drawn in
 * colour 8 while the others use 15. Inactive: the name is measured with func_800492A0
 * and drawn centred at baseY + 10 in colour 15.
 *
 * @par Likely role
 * The name field of the front-end player-setup page, one widget per player, driven by
 * the same action codes as the neighbouring front-end widgets.
 *
 * @par Unknown
 * Why D_8009F468[5] == 1 widens the cell by one pixel.
 *
 * @par Callers
 * No direct caller. Its address is the callback word at +4 of 0x70-byte records at
 * 0x8009BB00, 0x8009BB70 and 0x8009BBC4, so it is invoked through the front-end widget
 * list.
 */
void gHandleNameEntryGrid(s32 action, s32 baseX, s32 baseY) __asm__("func_8007ABBC");

void gHandleNameEntryGrid(s32 action, s32 baseX, s32 baseY)
{
    u8 text[8];
    s32 width;
    s32 height;
    s32 fontUnk8;
    s32 fontHeight;

    s32 idx;
    u8 *chars;
    s32 i;
    register s32 *base __asm__("$4");
    register s32 off __asm__("$3");

    idx = gActivePlayerIndex;
    switch (action) {
    case 3: {
        s8 *flag = &gPlayerMenuDepth[idx];
        if (*flag != 0) {
            gDecodeGlyphsToName(gPlayerNameTable + idx * 9, gNameEntryGlyphBuf + idx * 8, 8, gNameEntryAlphabet);
            *flag = 0;
        } else {
            *flag = 1;
            gScalarCursor[idx] = 0;
            gEncodeNameToGlyphs(gPlayerNameTable + idx * 9, gNameEntryGlyphBuf + idx * 8, 8, gNameEntryAlphabet, gPlayerNameLength);
        }
        break;
    }
    case 4:
        if (gPlayerMenuDepth[idx] != 0) {
            u8 *p;
            gPlayNonpositionalSoundEffect(0x46, 0);
            i = gScalarCursor[idx];
            chars = gNameEntryGlyphBuf + idx * 8;
            p = chars + i;
            if (*p != 0) {
                (*p)--;
            } else {
                *p = gPlayerNameLength - 1;
            }
        }
        break;
    case 5:
        if (gPlayerMenuDepth[idx] != 0) {
            u8 *p;
            gPlayNonpositionalSoundEffect(0x46, 0);
            i = gScalarCursor[idx];
            chars = gNameEntryGlyphBuf + idx * 8;
            p = chars + i;
            (*p)++;
            if (*p >= gPlayerNameLength) {
                *p = 0;
            }
        }
        break;
    case 1:
        if (gPlayerMenuDepth[idx] != 0) {
            s32 *p = &gScalarCursor[idx];
            (*p)++;
            if (*p >= 8) {
                *p = 0;
            }
            gPlayNonpositionalSoundEffect(0x47, 0);
        }
        break;
    case 2:
        if (gPlayerMenuDepth[idx] != 0) {
            s32 *p = &gScalarCursor[idx];
            (*p)--;
            if (*p < 0) {
                *p = 7;
            }
            gPlayNonpositionalSoundEffect(0x47, 0);
        }
        break;
    case 0: {
        s32 y = baseY + 10;
        if (gPlayerMenuDepth[idx] != 0) {
            struct FontEntry *font;
            s32 step;
            s32 *sel_ptr;

            font = gGetActiveFont();
            fontUnk8 = font->unk8;
            step = fontUnk8;
            if (gFontBankTable[5] == 1) {
                step++;
            }
            font = gGetActiveFont();
            chars = gNameEntryGlyphBuf + idx * 8;
            baseX -= step * 4;
            base = gScalarCursor;
            off = idx << 2;
            fontHeight = font->height;
            __asm__("" : "+r"(off) : "r"(fontHeight));
            sel_ptr = (s32 *)((u8 *)base + off);
            text[1] = 0;
            for (i = 0; i < 8; i++) {
                s32 textColor;
                text[0] = gNameEntryAlphabet[chars[i]];
                gMeasureTextExtent(text, &width, &height);
                if (i == *sel_ptr) {
                    register s32 boxX __asm__("$4");
                    register s32 boxKind __asm__("$7");
                    register u8 *pal __asm__("$3");
                    s32 c_idx;
                    s32 color;
                    boxX = baseX;
                    boxKind = 2;
                    pal = gHighlightShadeTable;
                    __asm__("" : "=r"(boxX), "=r"(boxKind), "=r"(pal) : "0"(boxX), "1"(boxKind), "2"(pal));
                    c_idx = (gGameClock[0] >> 4) & 7;
                    c_idx += (s32)pal;
                    color = *(u8 *)c_idx + 0x80;
                    gEmitFlatQuadRect(boxX, y + fontHeight + 2, fontUnk8, boxKind, 10, color, color, color);
                    textColor = 8;
                } else {
                    textColor = 0xF;
                }
                gDrawAlignedString(text, baseX + (fontUnk8 - width) / 2, y, textColor);
                baseX += step;
            }
        } else {
            u8 *str = gPlayerNameTable + idx * 9;
            gMeasureTextExtent(str, &fontUnk8, &fontHeight);
            gDrawAlignedString(str, baseX - fontUnk8 / 2, y, 0xF);
        }
        break;
    }
    }
}
